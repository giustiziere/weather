#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QDebug>
#include <QTextCodec>
#include <QXmlQuery>
#include <QBuffer>
#include <QTimer>
#include <QSettings>
#include <QFile>
#include <QDir>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    void createActions();
    QAction *settingsAction;
    QAction *aboutAction;
    QAction *exitAction;
    void getWeatherInfo();
    QString xmlCode;
    QString temperature;
    QString wind;
    QString windDirection;
    QString humidity;
    QString city;
    int timeoutUpdate;
    void showPopupWeather();
    void openSavedSettings();
    ~MainWindow();

public slots:
    void iconActivated(QSystemTrayIcon::ActivationReason);
    void showSettings();
    void showAbout();
    void exitApp();
    void updateWeather();
    void weatherRequestFinished(QNetworkReply*);

private slots:
    void on_exitSettingsButton_clicked();
    void on_saveSettingsButton_clicked();

    void on_clearCityButton_clicked();

    void on_clearTimeoutButton_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
