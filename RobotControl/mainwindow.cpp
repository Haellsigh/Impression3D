#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QNetworkDatagram>
#include <QSettings>

#include "log.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    createLanguageMenu();
    selectDefaultLanguage();

    // Setup progress bar
    m_ePrintProgress = new QProgressBar;
    ui->statusbar->addPermanentWidget(m_ePrintProgress, 1);
    m_ePrintProgress->setTextVisible(true);
    m_ePrintProgress->setAlignment(Qt::AlignHCenter);
    m_ePrintProgress->setFormat(tr("Print status: %p% (%v steps / %m)"));

    // Add the robot status text in the status bar
    m_lVRobotStatus = new QLabel(this);
    m_lVRobotStatus->setMinimumWidth(180);
    m_lVRobotStatus->setMargin(5);
    ui->statusbar->addPermanentWidget(m_lVRobotStatus);
    // No error
    requestStatus(dx200::RequestStatus());

    setupLogging();

    initConnections();

    m_timeoutPositionRead.setSingleShot(false);
    m_timeoutPositionRead.setInterval(500);
    m_timeoutPositionRead.start();

    m_interpreter.setClient(&m_client);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initConnections()
{
    // Request status response
    connect(&m_client, &dx200::HSEClient::requestStatus,
            this, &MainWindow::requestStatus);
    // Status information response
    connect(&m_client, &dx200::HSEClient::readStatusInformation,
            this, &MainWindow::handleStatusInformation);
    // Robot position response
    connect(&m_client, &dx200::HSEClient::readPositionCartesian,
            this, &MainWindow::handleRobotCartesianPosition);

    connect(&m_interpreter, &dx200::GCodeInterpreter::finishedLine, [&]() {
        m_ePrintProgress->setValue(m_ePrintProgress->value() + 1);
    });

    // Request position data on timeout
    connect(&m_timeoutPositionRead, &QTimer::timeout, [&]() {
        qWarning() << tr("MainWindow timeout: Didn't receive robot position in time (500ms)");
        m_client.robotPositionRead();
    });

    // Connect spinAxisI to sliderAxisI
    auto spinSignal = qOverload<int>(&QSpinBox::valueChanged);
    connect(ui->spinAxis1, spinSignal, ui->sliderAxis1, &QSlider::setValue);
    connect(ui->spinAxis2, spinSignal, ui->sliderAxis2, &QSlider::setValue);
    connect(ui->spinAxis3, spinSignal, ui->sliderAxis3, &QSlider::setValue);
    connect(ui->spinAxis4, spinSignal, ui->sliderAxis4, &QSlider::setValue);
    connect(ui->spinAxis5, spinSignal, ui->sliderAxis5, &QSlider::setValue);
    connect(ui->spinAxis6, spinSignal, ui->sliderAxis6, &QSlider::setValue);

    // Connect sliderAxisI to spinAxisI
    connect(ui->sliderAxis1, &QSlider::valueChanged, ui->spinAxis1, &QSpinBox::setValue);
    connect(ui->sliderAxis2, &QSlider::valueChanged, ui->spinAxis2, &QSpinBox::setValue);
    connect(ui->sliderAxis3, &QSlider::valueChanged, ui->spinAxis3, &QSpinBox::setValue);
    connect(ui->sliderAxis4, &QSlider::valueChanged, ui->spinAxis4, &QSpinBox::setValue);
    connect(ui->sliderAxis5, &QSlider::valueChanged, ui->spinAxis5, &QSpinBox::setValue);
    connect(ui->sliderAxis6, &QSlider::valueChanged, ui->spinAxis6, &QSpinBox::setValue);
}

void MainWindow::requestStatus(dx200::RequestStatus status)
{
    QString errorMessage;

    if (status.status != 0x00) {
        if (status.status == 0) {
            errorMessage = tr("Error");
        } else {
            uint32_t addedStatus = ((uint16_t)status.status1 << 16)
                                   + ((uint16_t)status.status2 << 0);
            errorMessage = tr("Error: code %1, added code: %2").arg(status.status, 0, 16).arg(addedStatus, 0, 16);
        }
        m_lVRobotStatus->setStyleSheet("background-color: rgb(200, 0, 0);");
    } else {
        errorMessage = tr("No errors");
        m_lVRobotStatus->setStyleSheet("background-color: rgb(0, 200, 0);");
    }

    m_lVRobotStatus->setText(errorMessage);
}

void MainWindow::handleStatusInformation(dx200::StatusInformation info)
{
    auto boolToString = [](bool value) {
        return value ? QString("true") : QString("false");
    };

    m_servoOn = info.Servo();

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

void MainWindow::handleRobotCartesianPosition(dx200::Movement::Cartesian position)
{
    ui->lVAxis1->setNum(position.x);
    ui->lVAxis2->setNum(position.y);
    ui->lVAxis3->setNum(position.z);
    ui->lVAxis4->setNum(position.tx);
    ui->lVAxis5->setNum(position.ty);
    ui->lVAxis6->setNum(position.tz);

    // Request another read
    m_client.robotPositionRead();
    m_timeoutPositionRead.start();
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

    QSettings settings;
    settings.setValue("language", rLanguage);
}

void MainWindow::createLanguageMenu()
{
    QActionGroup* langGroup = new QActionGroup(ui->menuLanguage);
    langGroup->setExclusive(true);

    connect(langGroup, &QActionGroup::triggered, this, &MainWindow::slotLanguageChanged);

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("RobotControl_*.qm"));

    for (int i = 0; i < fileNames.size(); i++) {
        qDebug() << tr("Loading language %1").arg(fileNames.at(i));
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
    }
}

void MainWindow::selectDefaultLanguage()
{
    QString defaultLocale = QLocale::system().name();       // ex: "de_DE"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // ex: "de"

    QSettings settings;
    QString lastLanguage = settings.value("language", defaultLocale).toString();

    loadLanguage(lastLanguage);
}

void MainWindow::setupLogging()
{
    if (!m_actionOpenLog)
        m_actionOpenLog = new QAction(tr("Logs"), ui->menubar);
    connect(m_actionOpenLog, &QAction::triggered, &m_logWidget, &LogWidget::show);
    ui->menubar->addAction(m_actionOpenLog);

    m_logWidget.setAsMessageHandler();
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    m_logWidget.close();
    QMainWindow::closeEvent(event);
}

void MainWindow::on_bSendPosition_clicked()
{
    using namespace dx200;

    int X  = ui->spinAxis1->value();
    int Y  = ui->spinAxis2->value();
    int Z  = ui->spinAxis3->value();
    int Tx = ui->spinAxis4->value();
    int Ty = ui->spinAxis5->value();
    int Tz = ui->spinAxis6->value();

    Movement::Cartesian mvtData;
    mvtData.robotNo        = 1;
    mvtData.stationNo      = 0; // TODO: Should be 1
    mvtData.classification = Movement::SpeedClassification::CARTESIAN_TRANSLATION;
    mvtData.speed          = 1000; // 10cm/s
    mvtData.coordinate     = 17;   // 17 = Robot coordinates

    qDebug() << "x: " << X;
    qDebug() << "y: " << Y;
    qDebug() << "z: " << Z;
    qDebug() << "tx: " << Tx;
    qDebug() << "ty: " << Ty;
    qDebug() << "tz: " << Tz;

    mvtData.x  = X;
    mvtData.y  = Y;
    mvtData.z  = Z;
    mvtData.tx = Tx;
    mvtData.ty = Ty;
    mvtData.tz = Tz;
    // The rest of the members are zero-initialized

    // Send command to robot
    m_client.moveCartesian(Movement::ABSOLUTE_JOINT, mvtData);
}

void MainWindow::on_bSetPO_clicked()
{
    double ox = 0, oy = 0, oz = 0;

    ox = ui->lVAxis1->text().toDouble() / 1000.;
    oy = ui->lVAxis2->text().toDouble() / 1000.;
    oz = ui->lVAxis3->text().toDouble() / 1000.;

    m_interpreter.setUserFrame(ox, oy, oz);

    ui->lVPOX->setText(QStringLiteral("x: %1 mm").arg(ox));
    ui->lVPOY->setText(QStringLiteral("y: %1 mm").arg(oy));
    ui->lVPOZ->setText(QStringLiteral("z: %1 mm").arg(oz));

    ui->containerStep1->setStyleSheet("QWidget#containerStep1 {\n  background-color: rgba(0, 255, 0, 30);\n}");
}

void MainWindow::on_bPrint_clicked()
{
    if (!ui->eFile->text().isEmpty()) {
        int totalSteps = m_interpreter.executeFile(ui->eFile->text());
        m_ePrintProgress->setMaximum(totalSteps - 1);
        m_ePrintProgress->reset();
    }
}

void MainWindow::on_bBrowseFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select GCode file"), QDir::currentPath(), tr("GCode Files (*.g *.gc *.gcode)"));

    if (!fileName.isEmpty()) {
        ui->eFile->setText(fileName);

        ui->containerStep2->setStyleSheet("QWidget#containerStep2 {\n  background-color: rgba(0, 255, 0, 30);\n}");
    }
}

void MainWindow::on_bStop_clicked()
{
    m_interpreter.stopExecution();
}

void MainWindow::on_bServoToggle_clicked()
{
    m_servoOn = !m_servoOn;
    m_client.servo(m_servoOn);
}

void MainWindow::on_pushButton_toggled(bool checked)
{
    m_client
}
