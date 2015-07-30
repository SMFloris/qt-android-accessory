/*
 * Copyright (C) 2015 Floris-Andrei Stoica-Marcu <floris.sm@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef KDECONNECT_USBSERVER_H
#define KDECONNECT_USBSERVER_H

#include <sched.h>
#include <unistd.h>

#include <QObject>
#include <QTimer>
#include <QMap>
#include <QPair>

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
    void send(unsigned char *msg, unsigned int size, libusb_device *dest);

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