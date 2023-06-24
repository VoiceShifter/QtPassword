#include "QtPasswordManager.h"

static sqlite3* db; //pointer to database
sqlite3_stmt* Statement = nullptr; //pointer to statement


QtPasswordManager::QtPasswordManager(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	char* ContainerForError = nullptr; //pointer to container for error




	sqlite3_open("Passwords.db", &db);
	int Code = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS Passwords (Source TEXT, Password TEXT)", NULL, NULL, &ContainerForError);
	if (Code != SQLITE_OK)
	{
		std::cout << "Error\n" << ContainerForError;
	}






	connect(ui.WritingPasswordButton, SIGNAL(clicked()), this, SLOT(WritingPasswordButtonClicked()));
	std::map<std::string, std::string>* aFileMap = PushPasswordsToMap(db, Statement);
	ShowPasswordLineScroll(aFileMap);

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

	std::map<std::string, std::string>* aFileMap = PushPasswordsToMap(db, Statement);
	aFileMap->insert_or_assign(SoucePassword, PasswordItself); //adding unencrypted password to map before writing encrypter to file to skip 1 function

	AddPassword(SoucePassword, PasswordItself, db);

	DeletePasswordLinesScroll(aFileMap->size());
	ShowPasswordLineScroll(aFileMap);

};

void QtPasswordManager::DeletePasswordLinesScroll(unsigned int SizeOfMap)
{
	ui.scrollArea->widget()->deleteLater();
}

void QtPasswordManager::DeletePasswordLines(unsigned int SizeOfMap)
{

	QVBoxLayout* LayoutForText = qobject_cast<QVBoxLayout*>(ui.Resourses->layout());
	QVBoxLayout* LayoutForPasswordButtons = qobject_cast<QVBoxLayout*>(ui.PasswordsButtons->layout());

	for (size_t Counter{ SizeOfMap }; Counter >= 2; Counter--)
	{

		LayoutForText->itemAt(Counter)->widget()->deleteLater();
		LayoutForPasswordButtons->itemAt(Counter)->widget()->deleteLater();

	}

}


void QtPasswordManager::ShowPasswordLine(std::map<std::string, std::string>* FileMap)
{
	QVBoxLayout* LayoutForText = qobject_cast<QVBoxLayout*>(ui.Resourses->layout());
	QVBoxLayout* LayoutForPasswordButtons = qobject_cast<QVBoxLayout*>(ui.PasswordsButtons->layout());
	for (auto Iterator{ FileMap->begin() }; Iterator != FileMap->end(); ++Iterator)
	{
		QLabel* PasswordSourceWidget = new QLabel(Iterator->first.c_str());
		QFont SourceFont{ PasswordSourceWidget->font() };
		SourceFont.setPointSizeF(13);
		PasswordSourceWidget->setFont(SourceFont);

		LayoutForText->insertWidget(2, PasswordSourceWidget);

		QPushButton* PasswordButtonWidget = new QPushButton("Copy password");
		LayoutForPasswordButtons->insertWidget(2, PasswordButtonWidget);

		connect(PasswordButtonWidget, &QPushButton::clicked,
			this, [=]() { CopingPassword(Iterator->second); });
	}
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


	std::fstream aFile(PathToPasswordFile, std::fstream::app | std::fstream::in);
	if (!aFile.is_open())
	{
		std::cerr << "cant open file\n";
	}

	IncryptPassword(Password);

	////////////////////
	int Status = sqlite3_exec(db, ("INSERT INTO Passwords VALUES ('" + Source + "', '" + Password + "')").c_str(), NULL, NULL, NULL);
	if (Status != SQLITE_OK)
	{
		std::cout << "Error\n";
	}
	////////////////////

	aFile << Source << ' ' << Password << '\n';
	aFile.close();

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
	std::fstream aFile(PathToPasswordFile, std::fstream::in | std::fstream::app);
	if (!aFile.is_open())
	{
		exit(1);
	}

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





	while (aFile >> aSource >> aPassword)
	{

		aPasswordBook->insert(std::make_pair(aSource, DecryptPassword(aPassword)));
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


