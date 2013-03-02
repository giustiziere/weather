#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEditCity->setInputMask("AAAAAAAAAAAAAAAAAAAA");
    ui->lineEditUpdatePeriod->setInputMask("000");
    trayIcon = new QSystemTrayIcon(this);
    trayMenu = new QMenu(this);
    QIcon icon = QIcon(":/qresource/warm.png");
    trayIcon->setIcon(icon);
    setWindowIcon(icon);

    manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),this, SLOT(weatherRequestFinished(QNetworkReply*)));

    createActions();
    openSavedSettings();
    getWeatherInfo();

    trayIcon->show();

    trayMenu->addAction(settingsAction);
    trayMenu->addAction(aboutAction);
    trayMenu->addSeparator();
    trayMenu->addAction(exitAction);
    trayIcon->setContextMenu(trayMenu);
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateWeather()));
    timer->start(timeoutUpdate * 1000);
}

MainWindow::~MainWindow()
{
    delete trayMenu;
    delete trayIcon;
    delete settingsAction;
    delete aboutAction;
    delete exitAction;
    delete manager;
    delete ui;
}

void MainWindow::updateWeather()
{
    getWeatherInfo();
    if (ui->checkShowPopupAtUpdate->isChecked()) showPopupWeather();
}

void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::MiddleClick:
        if (!this->isVisible()) {
            this->show();
        } else {
            this->hide();
        }
        break;
    case QSystemTrayIcon::Trigger:
        showPopupWeather();
        break;
    default:
        break;
    }
}

void MainWindow::showSettings()
{
    this->show();

}

void MainWindow::showAbout()
{
    QMessageBox::information(0, "About", "Author: Serji");
}

void MainWindow::exitApp()
{
    QApplication::quit();
}

void MainWindow::createActions()
{
    settingsAction = new QAction("Settings", this);
    connect(settingsAction, SIGNAL(triggered()), this, SLOT(showSettings()));
    aboutAction = new QAction("About", this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(showAbout()));
    exitAction = new QAction("Exit", this);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(exitApp()));
}

void MainWindow::getWeatherInfo()
{
    QNetworkRequest requestWeather;
    requestWeather.setUrl(QUrl("http://free.worldweatheronline.com/feed/weather.ashx?q=" + city + "&format=xml&num_of_days=2&key=8ce57ec61d152703120709"));
    requestWeather.setRawHeader("Host", "free.worldweatheronline.com");
    requestWeather.setRawHeader("Keep-Alive", "115");
    requestWeather.setRawHeader("Connection", "keep-alive");
    manager->get(requestWeather);
}

void MainWindow::weatherRequestFinished(QNetworkReply *reply)
{
    QByteArray answer = reply->read(reply->bytesAvailable());
    /*QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    xmlCode = codec->toUnicode(answer);
    qWarning() << xmlCode;*/

    QXmlQuery xmlQuery;
    QBuffer buffer(&answer);
    buffer.open(QIODevice::ReadOnly);
    xmlQuery.bindVariable("myDocument", &buffer);

    QString errorCity;
    xmlQuery.setQuery("doc($myDocument)/data/error/msg/string()");
    xmlQuery.evaluateTo(&errorCity);
    qDebug() << errorCity;
    if (errorCity.left(errorCity.length() - 1) != "") {
        QMessageBox::warning(0, "Ошибка!", "Невозможно получить данные о городе!\nПроверьте правильность ввода названия.\nErrorLog: " + errorCity);
        this->show();
        ui->lineEditCity->clear();
        buffer.close();
        reply->deleteLater();
        answer.clear();
        return;
    }

    //city
    xmlQuery.setQuery("doc($myDocument)/data/request/query/string()");
    xmlQuery.evaluateTo(&city);
    city = city.left(city.length() - 1);

    //temperature
    xmlQuery.setQuery("doc($myDocument)/data/current_condition/temp_C/string()");
    xmlQuery.evaluateTo(&temperature);
    temperature = temperature.left(temperature.length() - 1);

    //wind
    xmlQuery.setQuery("doc($myDocument)/data/current_condition/windspeedKmph/string()");
    xmlQuery.evaluateTo(&wind);
    wind = wind.left(wind.length() - 1);

    //wind direction
    xmlQuery.setQuery("doc($myDocument)/data/current_condition/winddir16Point/string()");
    xmlQuery.evaluateTo(&windDirection);
    windDirection = windDirection.left(windDirection.length() - 1);

    //humidity
    xmlQuery.setQuery("doc($myDocument)/data/current_condition/humidity/string()");
    xmlQuery.evaluateTo(&humidity);
    humidity = humidity.left(humidity.length() - 1);

    qDebug() << city;
    qDebug() << temperature;
    qDebug() << wind;
    qDebug() << windDirection;
    qDebug() << humidity;

    buffer.close();
    reply->deleteLater();
    answer.clear();
}

void MainWindow::weatherRequestFinishedTest(QNetworkReply *reply)
{
    QByteArray answer = reply->read(reply->bytesAvailable());
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    xmlCode = codec->toUnicode(answer);
    qWarning() << xmlCode;

    city = "test";

    temperature = "test";

    wind = "test";

    windDirection = "test";

    humidity = "test";

    qDebug() << city;
    qDebug() << temperature;
    qDebug() << wind;
    qDebug() << windDirection;
    qDebug() << humidity;

    reply->deleteLater();
}

void MainWindow::showPopupWeather()
{
    trayIcon->showMessage("Погода в " + city, "Сейчас " + temperature + " C\n" + "Скорость ветра " + wind + " км/ч\n" + "Направление ветра " + windDirection + "\n" + "Влажность " + humidity + "\%", QSystemTrayIcon::Information, 10000);
}

void MainWindow::on_exitSettingsButton_clicked()
{
    QApplication::closeAllWindows();
}

void MainWindow::on_saveSettingsButton_clicked()
{
    if (ui->lineEditCity->text() == "" || ui->lineEditUpdatePeriod->text() == "") {
        QMessageBox::warning(0, "Ошибка!", "Не заполнено название города или значение периода обновления!");
        return;
    } else {
        QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
        settings.setValue("city", ui->lineEditCity->text());
        settings.setValue("timeout", ui->lineEditUpdatePeriod->text());
        settings.setValue("checkbaloon", ui->checkShowPopupAtUpdate->isChecked() ? "1" : "0");
        settings.setValue("checkproxy", ui->checkProxy->isChecked() ? "1" : "0");
        settings.setValue("proxyhost", ui->lineEditProxyServer->text());
        settings.setValue("proxyport", ui->lineEditProxyPort->text());
        settings.setValue("proxylogin", ui->lineEditProxyLogin->text());
        settings.setValue("proxypassword", ui->lineEditProxyPassword->text());
        openSavedSettings();
        getWeatherInfo();
    }
}

void MainWindow::openSavedSettings()
{
    QSettings settings(QCoreApplication::applicationDirPath() + "/config.ini", QSettings::IniFormat);
    city = settings.value("city").toString();
    ui->lineEditCity->setText(city);
    timeoutUpdate = settings.value("timeout").toInt();
    QString timeoutUpdateStr = settings.value("timeout").toString();
    ui->lineEditUpdatePeriod->setText(timeoutUpdateStr);
    int checkbaloon = settings.value("checkbaloon").toInt();
    if (checkbaloon == 0) {
        ui->checkShowPopupAtUpdate->setChecked(false);
    } else {
        ui->checkShowPopupAtUpdate->setChecked(true);
    }
    int checkproxy = settings.value("checkproxy").toInt();
    if (checkproxy == 0) {
        ui->checkProxy->setChecked(false);
    } else {
        ui->checkProxy->setChecked(true);
    }
    proxyHost = settings.value("proxyhost").toString();
    ui->lineEditProxyServer->setText(proxyHost);
    proxyPort = settings.value("proxyport").toInt();
    ui->lineEditProxyPort->setText(settings.value("proxyport").toString());
    proxyLogin = settings.value("proxylogin").toString();
    ui->lineEditProxyLogin->setText(proxyLogin);
    proxyPassword = settings.value("proxypassword").toString();
    ui->lineEditProxyPassword->setText(proxyPassword);
    if (!ui->checkProxy->isChecked()) {
        proxy.setType(QNetworkProxy::NoProxy);
        QNetworkProxy::setApplicationProxy(proxy);
    } else {
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(proxyHost);
        proxy.setPort(proxyPort);
        proxy.setUser(proxyLogin);
        proxy.setPassword(proxyPassword);
        QNetworkProxy::setApplicationProxy(proxy);
    }

    qDebug() << city;
    qDebug() << proxyHost;
    qDebug() << proxyPort;
    qDebug() << proxyLogin;
    qDebug() << proxyPassword;
    qDebug() << proxy;
    qDebug() << proxy.user();
}

void MainWindow::on_clearCityButton_clicked()
{
    ui->lineEditCity->clear();
}

void MainWindow::on_clearTimeoutButton_clicked()
{
    ui->lineEditUpdatePeriod->clear();
}

void MainWindow::createTrayIcon()
{

}
