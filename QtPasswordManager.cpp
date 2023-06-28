#include "QtPasswordManager.h"

static sqlite3* db; //pointer to database
sqlite3_stmt* Statement = nullptr; //pointer to statement


QtPasswordManager::QtPasswordManager(QWidget* parent)
	: QMainWindow(parent)
{	
	ui.setupUi(this);
	char* ContainerForError = nullptr; //pointer to container for error

	sqlite3_open("Passwords.db", &db);
	signed int Code = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Passwords (Source TEXT PRIMARY KEY, Password TEXT);", NULL, NULL, &ContainerForError);
	if (Code != SQLITE_OK)
	{
		std::cout << "Error\n" << ContainerForError;
	}


	connect(ui.WritingPasswordButton, SIGNAL(clicked()), this, SLOT(WritingPasswordButtonClicked()));
	std::map<std::string, std::string>* aDbMap = PushPasswordsToMap(db, Statement);
	ShowPasswordLineScroll(aDbMap);

}


void QtPasswordManager::WritingPasswordButtonClicked()
{
	std::string SoucePassword{ ui.PasswordSource->text().toStdString() };
	std::string PasswordItself{ ui.PasswordItself->text().toStdString() };
	ui.PasswordSource->setText("");
	ui.PasswordItself->setText("");
	if (SoucePassword == "" || PasswordItself == "")
	{
		return;
	}

	std::map<std::string, std::string>* aDbMap = PushPasswordsToMap(db, Statement);
	aDbMap->insert_or_assign(SoucePassword, PasswordItself); //adding unencrypted password to map before writing encrypter to file to skip 1 function

	AddPassword(SoucePassword, PasswordItself, db);

	DeletePasswordLinesScroll(aDbMap->size());
	delete aDbMap;
	aDbMap = PushPasswordsToMap(db, Statement);

	ShowPasswordLineScroll(aDbMap);

};


void QtPasswordManager::DeletePasswordLinesScroll(unsigned int SizeOfMap)
{
	ui.scrollArea->widget()->deleteLater();
}


void QtPasswordManager::ShowPasswordLineScroll(std::map<std::string, std::string>* FileMap)
{
	QWidget* ScrollWidget = new QWidget;
	ScrollWidget->setLayout(new QVBoxLayout);

	ScrollWidget->layout()->setAlignment(Qt::AlignTop);
	ui.scrollArea->setWidget(ScrollWidget);

	for (auto Iterator{ FileMap->begin() }; Iterator != FileMap->end(); ++Iterator)
	{
		QPushButton* PasswordButtonWidget = new QPushButton(Iterator->first.c_str());
		PasswordButtonWidget->setStyleSheet("font:bold; font-size: 16px; background-color: rgb(100, 114, 170)");
		ScrollWidget->layout()->addWidget(PasswordButtonWidget);
		connect(PasswordButtonWidget, &QPushButton::clicked,
			this, [=]() { CopingPassword(Iterator->second); });
	}

}
void QtPasswordManager::CopingPassword(std::string Password)
{
	QClipboard* Clipboard = QApplication::clipboard();
	Clipboard->setText(QString{ Password.c_str() }, QClipboard::Clipboard);
}


void QtPasswordManager::DelitingWithoutClicks()
{
	delete sender();
}


QtPasswordManager::~QtPasswordManager()
{}


void AddPassword(std::string Source, std::string Password, sqlite3* db)
{

	if (Password == "0")
	{
		sqlite3_exec(db, ("DELETE FROM Passwords WHERE Source = '" + Source + "'").c_str(), NULL, NULL, NULL);

		return;
	}

	IncryptPassword(Password);

	sqlite3_prepare_v2(db, ("SELECT * FROM Passwords WHERE Source = '" + Source + "'").c_str(), -1, &Statement, NULL);
	if (sqlite3_step(Statement) == SQLITE_ROW)	
	{
		sqlite3_exec(db, ("UPDATE Passwords SET Password = '" + Password + "' WHERE Source = '" + Source + "'").c_str(), NULL, NULL, NULL);
		return;
	}


	signed int Status = sqlite3_exec(db, ("INSERT INTO Passwords VALUES ('" + Source + "', '" + Password + "')").c_str(), NULL, NULL, NULL);
	if (Status != SQLITE_OK)
	{
		std::cout << "Error\n";
	}

	

}



void IncryptPassword(std::string& aPassword)
{
	for (char& Character : aPassword)
	{
		Character += 13;
	}
}


std::map<std::string, std::string>* PushPasswordsToMap(sqlite3* db, sqlite3_stmt* Statement)
{
	std::string aPassword{}, aSource{};

	std::map<std::string, std::string>* aPasswordBook = new std::map<std::string, std::string>;
	sqlite3_prepare_v2(db, "SELECT * FROM Passwords", -1, &Statement, NULL); //getting data from database
	while (sqlite3_step(Statement) != SQLITE_DONE)
	{
		aSource = reinterpret_cast<const char*>(sqlite3_column_text(Statement, 0));
		aPassword = reinterpret_cast<const char*>(sqlite3_column_text(Statement, 1));
		DecryptPassword(aPassword);
		aPasswordBook->insert(std::make_pair(aSource, aPassword));
	}

	return aPasswordBook;

}


std::string DecryptPassword(std::string& aPassword)
{
	for (char& Character : aPassword)
	{
		Character -= 13;
	}
	return aPassword;
}


