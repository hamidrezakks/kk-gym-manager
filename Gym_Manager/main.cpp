#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QRect tempRect = QApplication::desktop()->screenGeometry();
    w.move(tempRect.width()/2 - w.width()/2,
           tempRect.height()/2 - w.height()/2);
    w.show();

    return a.exec();
}
