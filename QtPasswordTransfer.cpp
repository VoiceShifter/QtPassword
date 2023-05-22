#include "QtPasswordTransfer.h"

QtPasswordTransfer::QtPasswordTransfer(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.WritingPasswordButton, SIGNAL(clicked()), this, SLOT(WritingPasswordButtonClicked()));
	std::map<std::string, std::string>* aFileMap = PushPasswordsToMap();
	ShowPasswordLineScroll(aFileMap);
}


void QtPasswordTransfer::WritingPasswordButtonClicked()
{
	std::string SoucePassword{ ui.PasswordSource->text().toStdString() };
	std::string PasswordItself{ ui.PasswordItself->text().toStdString() };
	ui.PasswordSource->setText("");
	ui.PasswordItself->setText("");
	if (SoucePassword == "" || PasswordItself == "")
	{
		return;
	}

	std::map<std::string, std::string>* aFileMap = PushPasswordsToMap();
	aFileMap->insert_or_assign(SoucePassword, PasswordItself); //adding unencrypted password to map before writing encrypter to file to skip 1 function

	AddPassword(SoucePassword, PasswordItself);

	DeletePasswordLinesScroll(aFileMap->size());
	ShowPasswordLineScroll(aFileMap);

};

void QtPasswordTransfer::DeletePasswordLinesScroll(unsigned int SizeOfMap)
{
	ui.scrollArea->widget()->deleteLater();
}
void QtPasswordTransfer::DeletePasswordLines(unsigned int SizeOfMap)
{

	QVBoxLayout* LayoutForText = qobject_cast<QVBoxLayout*>(ui.Resourses->layout());
	QVBoxLayout* LayoutForPasswordButtons = qobject_cast<QVBoxLayout*>(ui.PasswordsButtons->layout());

	for (size_t Counter{ SizeOfMap }; Counter >= 2; Counter--)
	{

		LayoutForText->itemAt(Counter)->widget()->deleteLater();
		LayoutForPasswordButtons->itemAt(Counter)->widget()->deleteLater();

	}

}


void QtPasswordTransfer::ShowPasswordLine(std::map<std::string, std::string>* FileMap)
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
void QtPasswordTransfer::ShowPasswordLineScroll(std::map<std::string, std::string>* FileMap)
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
void QtPasswordTransfer::CopingPassword(std::string Password)
{
	QClipboard* Clipboard = QApplication::clipboard();
	Clipboard->setText(QString{ Password.c_str() }, QClipboard::Clipboard);
}

void QtPasswordTransfer::DelitingWithoutClicks()
{
	delete sender();
}

QtPasswordTransfer::~QtPasswordTransfer()
{}


void AddPassword(std::string Source, std::string Password)
{

	std::fstream aFile(PathToPasswordFile, std::fstream::app | std::fstream::in);
	if (!aFile.is_open())
	{
		std::cerr << "cant open file\n";
	}
	IncryptPassword(Password);
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


std::map<std::string, std::string>* PushPasswordsToMap()
{
	std::string aPassword{}, aSource{};
	std::fstream aFile(PathToPasswordFile, std::fstream::in | std::fstream::app);
	if (!aFile.is_open())
	{
		exit(1);
	}
	std::map<std::string, std::string>* aPasswordBook = new std::map<std::string, std::string>;
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


