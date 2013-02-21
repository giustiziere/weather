#include <QtGui/QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QTextCodec *cyrillicCodec = QTextCodec::codecForName("CP-1251");
    QTextCodec::setCodecForTr(cyrillicCodec);
    QTextCodec::setCodecForLocale(cyrillicCodec);
    QTextCodec::setCodecForCStrings(cyrillicCodec);
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Giustiziere Prod.");
    QCoreApplication::setApplicationName("CheckYourWeather");
    MainWindow w;
    a.setQuitOnLastWindowClosed(false);
    w.hide();

    return a.exec();
}
