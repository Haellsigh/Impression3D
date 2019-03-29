#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QDir>
#include <QNetworkDatagram>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createLanguageMenu();

    initConnections();

    // Send the first request
    m_station.StatusInformationRead();

    // Periodic requests
    m_timer.setSingleShot(false);
    m_timer.start(50);
    connect(&m_timer, &QTimer::timeout, &m_station, &HSE::Client::StatusInformationRead);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::requeteStatus(HSE::RequestStatus status)
{
    if (status.Status == 0x00) {
        ui->lVErrorGlobal->setText(tr("No error"));
    } else {
        ui->lVErrorGlobal->setText(tr("Error: code %1").arg(status.Status, 0, 16));
    }
}

void MainWindow::StatusInformationReceived(HSE::StatusInformation info)
{
    auto boolToString = [](bool value) {
        return value ? QString("true") : QString("false");
    };

    ui->lVServo->setText(boolToString(info.Servo()));
    ui->lVAlarming->setText(boolToString(info.Alarming()));
    ui->lVAC->setText(boolToString(info.AutomaticAndContinuous()));
    ui->lVCommandRemote->setText(boolToString(info.CommandRemote()));
    ui->lVCycle1->setText(boolToString(info.Cycle1()));
    ui->lVError->setText(boolToString(info.ErrorOcurring()));
    ui->lVIGSO->setText(boolToString(info.InGuardSafeOperation()));
    ui->lVHoldPP->setText(boolToString(info.InHoldByPP()));
    ui->lVHoldCmd->setText(boolToString(info.InHoldByCmd()));
    ui->lVHoldExt->setText(boolToString(info.InHoldByExt()));
    ui->lVPlay->setText(boolToString(info.Play()));
    ui->lVRunning->setText(boolToString(info.Running()));
    ui->lVStep->setText(boolToString(info.Step()));
    ui->lVTeach->setText(boolToString(info.Teach()));
}

void MainWindow::initConnections()
{
    // Connect request status response
    connect(&m_station, &HSE::Client::requestStatus, this, &MainWindow::requeteStatus);
    connect(&m_station, &HSE::Client::getStatusInformationRead, this, &MainWindow::StatusInformationReceived);
}

void MainWindow::changeEvent(QEvent* event)
{
    if (event == nullptr)
        return;

    switch (event->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

    case QEvent::LocaleChange: {
        QString locale = QLocale::system().name();
        locale.truncate(locale.lastIndexOf('_'));
        loadLanguage(locale);
    } break;
    }

    // Pass event
    QMainWindow::changeEvent(event);
}

void MainWindow::slotLanguageChanged(QAction* action)
{
    if (action == nullptr)
        return;

    loadLanguage(action->data().toString());
}

void switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    QString path = QApplication::applicationDirPath();
    path.append("/languages/");
    if (translator.load(path + filename)) //Here Path and Filename has to be entered because the system didn't find the QM Files else
        qApp->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if (m_currentLang == rLanguage)
        return;

    m_currentLang  = rLanguage;
    QLocale locale = QLocale(m_currentLang);
    QLocale::setDefault(locale);
    QString languageName = QLocale::languageToString(locale.language());
    switchTranslator(m_translator, QString("RobotControl_%1.qm").arg(rLanguage));
    switchTranslator(m_translatorQt, QString("qt_%1.qm").arg(rLanguage));
    ui->statusbar->showMessage(tr("Current language changed to %1").arg(languageName));
}

void MainWindow::createLanguageMenu()
{
    qDebug() << "createLanguageMenu";
    QActionGroup* langGroup = new QActionGroup(ui->menuLanguage);
    langGroup->setExclusive(true);

    connect(langGroup, &QActionGroup::triggered, this, &MainWindow::slotLanguageChanged);

    QString defaultLocale = QLocale::system().name();       // ex: "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // ex: "de"

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("RobotControl_*.qm"));

    for (int i = 0; i < fileNames.size(); i++) {
        qDebug() << "Loading language" << fileNames.at(i);
        QString locale;
        locale = fileNames.at(i);
        locale.truncate(locale.lastIndexOf('.'));
        locale.remove(0, locale.indexOf('_') + 1);

        QString lang = QLocale::languageToString(QLocale(locale).language());

        QAction* action = new QAction(lang, this);
        action->setCheckable(true);
        action->setData(locale);

        ui->menuLanguage->addAction(action);
        langGroup->addAction(action);

        // Select system language as current
        if (defaultLocale == locale) {
            action->setChecked(true);
        }
    }
}
