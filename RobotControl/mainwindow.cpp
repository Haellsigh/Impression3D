#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QNetworkDatagram>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_lVRobotStatus = new QLabel(this);
    m_lVRobotStatus->setMargin(5);
    ui->statusbar->addPermanentWidget(m_lVRobotStatus);
    updateRobotStatus(false);

    setupLogging();

    createLanguageMenu();

    initConnections();

    m_reader.decodeFile("test.gcode");

    // Request robot status at least once every 500ms
    m_timerStatusRequest.setInterval(1000);
    m_timerStatusRequest.start();
    // Send the first request
    m_station.statusInformationRead();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnections()
{
    // Request status response
    connect(&m_station, &HSE::Client::requestStatus, this, &MainWindow::requeteStatus);
    connect(&m_station, &HSE::Client::getStatusInformationRead, this, &MainWindow::StatusInformationReceived);

    // Send periodic requests
    connect(&m_timerStatusRequest, &QTimer::timeout, &m_station, &HSE::Client::statusInformationRead);
}

void MainWindow::requeteStatus(HSE::RequestStatus status)
{
    updateRobotStatus(status.Status == 0x00, status.Status);
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

    // Update information as fast as possible
    m_station.statusInformationRead();
    // Reset the 500ms timer
    m_timerStatusRequest.start();
}

void MainWindow::updateRobotStatus(bool error, int code)
{
    QString robotStatus;

    if (error) {
        if (code == 0) {
            robotStatus = tr("Error");
        } else {
            robotStatus = tr("Error: code %1").arg(code);
        }
        m_lVRobotStatus->setStyleSheet("background-color: rgb(200, 0, 0);");
    } else {
        robotStatus = tr("No error");
        m_lVRobotStatus->setStyleSheet("background-color: rgb(0, 200, 0);");
    }

    m_lVRobotStatus->setText(robotStatus);
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
    default:
        break;
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
        qInfo().noquote() << tr("Loading language") << fileNames.at(i);
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

void MainWindow::setupLogging()
{
    if (!m_actionOpenLog)
        m_actionOpenLog = new QAction("Logs", ui->menubar);
    connect(m_actionOpenLog, &QAction::triggered, &m_logWidget, &LogWidget::show);
    ui->menubar->addAction(m_actionOpenLog);

    m_logWidget.setAsMessageHandler();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_logWidget.close();
    QMainWindow::closeEvent(event);
}
