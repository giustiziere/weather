#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Giustiziere Prod.");
    QCoreApplication::setApplicationName("CheckYourWeather");
    MainWindow w;
    a.setQuitOnLastWindowClosed(false);
    w.hide();

    return a.exec();
}
