#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QTranslator>

#include "gcode/gcodereader.h"
#include "logwidget.h"
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
    void updateRobotStatus(bool error, int code = 0);

    void slotCurrentPosSelected(int posType);
    void slotMovePosSelected(int posType);

private:
    void initConnections();
    Ui::MainWindow* ui;

    // Robot status
    QLabel* m_lVRobotStatus = nullptr;

    HSE::Client m_station;
    gcode::Reader m_reader;

    QTimer m_timer;

    // Traductions
protected:
    void changeEvent(QEvent* event);
protected slots:
    void slotLanguageChanged(QAction* action);

private:
    void loadLanguage(const QString& rLanguage);
    void createLanguageMenu();

    QTranslator m_translator;
    QTranslator m_translatorQt;
    QString m_currentLang;
    QString m_langPath;

    // Logging
private:
    void setupLogging();
    LogWidget m_logWidget;
    QAction* m_actionOpenLog = nullptr;

    void closeEvent(QCloseEvent* event);
};

#endif // MAINWINDOW_H
