#include <QtGui/QApplication>
#include <QCoreApplication>
#include "monitor.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName("Monitor");
    QCoreApplication::setApplicationVersion("0.2pre");
    QCoreApplication::setOrganizationName("Instrument Development Laboratory University of Hawaii");
    QCoreApplication::setOrganizationDomain("http://www.phys.hawaii.edu/~idlab/");
    monitor w;
    w.show();
    return a.exec();
}
