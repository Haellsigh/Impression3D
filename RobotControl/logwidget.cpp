#include "logwidget.h"
#include "ui_logwidget.h"

#include <QApplication>
#include <QTime>

LogWidget* LogWidget::currentHandler = nullptr;

LogWidget::LogWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::LogWidget)
{
    ui->setupUi(this);
}

LogWidget::~LogWidget()
{
    delete ui;
}

void LogWidget::setAsMessageHandler()
{
    currentHandler = this;
    qInstallMessageHandler(handleMessages);
}

void LogWidget::handleMessages(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    if (currentHandler)
        currentHandler->handleMessagesImpl(type, context, msg);

    // Also send to default handler
    (*qInstallMessageHandler(0))(type, context, msg);
}

void LogWidget::log(QString message, QtMsgType type)
{
    QTextCharFormat newCharFormat = ui->logText->currentCharFormat();

    switch (type) {
    case QtDebugMsg:
        newCharFormat.setForeground(QBrush(Qt::magenta));
        break;
    case QtInfoMsg:
        newCharFormat.setForeground(QBrush(Qt::black));
        break;
    case QtWarningMsg:
        newCharFormat.setForeground(QBrush(Qt::darkYellow));
        break;
    case QtCriticalMsg:
        newCharFormat.setForeground(QBrush(Qt::red));
        break;
    case QtFatalMsg:
        newCharFormat.setForeground(QBrush(Qt::darkRed));
        break;
    }

    ui->logText->setCurrentCharFormat(newCharFormat);
    ui->logText->appendPlainText(QString("[%1] %2").arg(QTime::currentTime().toString("hh:mm:ss.zzz")).arg(message));
}

void LogWidget::handleMessagesImpl(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    //QByteArray localMsg = msg.toLocal8Bit();
    log(msg, type);
}
