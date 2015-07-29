#ifndef KDECONNECT_USBSERVER_H
#define KDECONNECT_USBSERVER_H

#include <stdlib.h>
#include <stdio.h>
#include <sched.h>
#include <unistd.h>

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QPair>
#include <usb.h>

extern "C" {
    #include <libusb-1.0/libusb.h>
}

class UsbDevice;

//the required things for the AndroidOpenAccessory API
struct AndroidAccessory {
    QString manufacturer, model, description, versionName, url, serial;
};

class UsbServer : public QObject
{
    Q_OBJECT

public:
    UsbServer();
    virtual ~UsbServer();

    int init();
    void stop();

    void attach(libusb_device *dev);
    void detach(libusb_device *dev);
    void receive(unsigned char *msg, unsigned int size);

public slots:
    void handleEvents();
    void send(unsigned char *msg, unsigned int size, UsbDevice *dest);

signals:
    void newConnection();
    void onReceived(unsigned char *msg, unsigned int size);

private:
    int accessory(libusb_device *dev);
    //valid Android devices
    QMap< QPair<uint16_t,uint16_t>, libusb_device_handle*> handles;
    AndroidAccessory info;
    QTimer checkTimeout;
};

#endif