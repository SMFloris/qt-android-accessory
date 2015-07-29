#include <QCoreApplication>
#include "usbserver.h"


int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    UsbServer *linuxandroidqt = new UsbServer();
    linuxandroidqt->init();
    return app.exec();
}
