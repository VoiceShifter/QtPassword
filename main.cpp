#include "QtPasswordManager.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QtPasswordManager w;
    w.show();
    return a.exec();
}
