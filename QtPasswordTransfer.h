#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_QtPasswordTransfer.h"

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


const std::string PathToPasswordFile{ "password.txt" };


class QtPasswordTransfer : public QMainWindow
{
	Q_OBJECT

public:
	QtPasswordTransfer(QWidget* parent = nullptr);
	~QtPasswordTransfer();
	void ShowPasswordLine(std::map<std::string, std::string>*);
	void ShowPasswordLineScroll(std::map<std::string, std::string>*);

	void DeletePasswordLines(unsigned int);
	void DeletePasswordLinesScroll(unsigned int);


private:
	Ui::QtPasswordTransferClass ui;

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
std::map<std::string, std::string>* PushPasswordsToMap();




void AddPassword(std::string, std::string);

