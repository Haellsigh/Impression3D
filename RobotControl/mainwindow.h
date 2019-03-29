#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>

#include "robot/hseclient.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void requeteStatus(HSE::RequestStatus status);

    void StatusInformationReceived(HSE::StatusInformation info);

private:
    Ui::MainWindow* ui;

    HSE::Client m_station;

    QTimer m_timer;
};

#endif // MAINWINDOW_H
