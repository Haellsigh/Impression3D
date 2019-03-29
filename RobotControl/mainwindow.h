#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTranslator>

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
};

#endif // MAINWINDOW_H
