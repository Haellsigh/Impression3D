#ifndef LOGWIDGET_H
#define LOGWIDGET_H

#include <QWidget>

namespace Ui {
class LogWidget;
}

class LogWidget : public QWidget {
    Q_OBJECT

public:
    explicit LogWidget(QWidget* parent = nullptr);
    ~LogWidget();

    void setAsMessageHandler();
    static void handleMessages(QtMsgType type, const QMessageLogContext& context, const QString& msg);

    void logInfo(QString message);
    void log(QString message, QtMsgType type = QtInfoMsg);

private:
    Ui::LogWidget* ui;

    static LogWidget* currentHandler;
    void handleMessagesImpl(QtMsgType type, const QMessageLogContext& context, const QString& msg);
};

#endif // LOGWIDGET_H
