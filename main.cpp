#include "QtPasswordTransfer.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{

    QApplication a(argc, argv);	
	
    QtPasswordTransfer w;	
	w.setWindowTitle("Password Manager");
    w.show();
    return a.exec();
}
