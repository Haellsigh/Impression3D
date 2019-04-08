#include "mainwindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName("RobotControl");
    QCoreApplication::setOrganizationDomain("ensil-ensci.unilim.fr");
    QCoreApplication::setApplicationName("ENSIL-ENSCI");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
