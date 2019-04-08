#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QTimer>
#include <QTranslator>

#include "gcode/gcodereader.h"
#include "logwidget.h"
#include "robot/gcodeinterpreter.h"
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
    void requestStatus(dx200::RequestStatus status);

    void handleStatusInformation(dx200::StatusInformation info);
    void updateRobotStatus(bool error, int code = 0);
    void handleRobotCartesianPosition(dx200::Movement::Cartesian position);

    void on_bSendPosition_clicked();

    void on_bSetPO_clicked();
    void on_bPrint_clicked();
    void on_bBrowseFile_clicked();

    void on_bStop_clicked();

private:
    void initConnections();
    Ui::MainWindow* ui;

    // Robot status label
    QLabel* m_lVRobotStatus        = nullptr;
    QProgressBar* m_ePrintProgress = nullptr;

    dx200::HSEClient m_client;
    dx200::GCodeInterpreter m_interpreter;
    gcode::Reader m_reader;

    ///////////////////////////////////////////////////////////////////////////
    /// Traductions
protected:
    void changeEvent(QEvent* event);
protected slots:
    void slotLanguageChanged(QAction* action);

private:
    void loadLanguage(const QString& rLanguage);
    void createLanguageMenu();
    void selectDefaultLanguage();

    QTranslator m_translator;
    QTranslator m_translatorQt;
    QString m_currentLang;
    QString m_langPath;

    ///////////////////////////////////////////////////////////////////////////
    /// Logging
private:
    void setupLogging();
    LogWidget m_logWidget;
    QAction* m_actionOpenLog = nullptr;

    void closeEvent(QCloseEvent* event);
};

#endif // MAINWINDOW_H
