#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QNetworkDatagram>

#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Connecte la réponse
    connect(&m_station, &HSE::Client::requestStatus, this, &MainWindow::requeteStatus);
    connect(&m_station, &HSE::Client::getStatusInformationRead, this, &MainWindow::StatusInformationReceived);

    // Envoie la requete
    m_station.StatusInformationRead();

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
        ui->lVErrorGlobal->setText("Aucune");
    } else {
        ui->lVErrorGlobal->setText("Erreur!");
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
