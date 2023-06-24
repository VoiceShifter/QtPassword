#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtPasswordManager.h"

# include <QtWidgets/QMainWindow>
# include <QTextStream>
# include <QFile>
# include <qdebug.h>
# include <QClipboard>

# include <iostream>
# include <fstream>
# include <map>
# include <vector>
# include <iterator>
# include "sqlite-amalgamation/sqlite3.h"


# include <string>



const std::string PathToPasswordFile{ "password.txt" };




class QtPasswordManager : public QMainWindow
{
	Q_OBJECT

public:
	QtPasswordManager(QWidget* parent = nullptr);
	~QtPasswordManager();
	void ShowPasswordLine(std::map<std::string, std::string>*);
	void ShowPasswordLineScroll(std::map<std::string, std::string>*);

	void DeletePasswordLines(unsigned int);
	void DeletePasswordLinesScroll(unsigned int);


private:
	Ui::QtPasswordManagerClass ui;

signals:
	void SendPassword();
	void DeleteDynamicWidget();
	void CopyPasswordClicked();

private slots:
	void WritingPasswordButtonClicked();
	void DelitingWithoutClicks();
	void CopingPassword(std::string);


};

std::string DecryptPassword(std::string& aPassword);
void IncryptPassword(std::string& aPassword);
std::map<std::string, std::string>* PushPasswordsToMap(sqlite3*, sqlite3_stmt*);
void AddPassword(std::string, std::string, sqlite3*);