#ifndef KDECONNECT_USBDEVICE_H
#define KDECONNECT_USBDEVICE_H

#include <QObject>
#include <QString>
#include <libusb-1.0/libusb.h>

class UsbDevice : public QObject
{
    Q_OBJECT

public:
    UsbDevice();
    virtual ~UsbDevice();
    
    void setVendorId(int vId);
    void setProductId(int pId);
    void setDev(libusb_device *dev);
    void setDeviceHandle(libusb_device_handle *devHandle);
    void setDevAccHandle(libusb_device_handle *devHandle);
    
    int vId();
    int pId();
    libusb_device *getDev();
    libusb_device_handle *getHandle();
    libusb_device_handle *getAccHandle();
    
private:
    int vendorId, productId;
    libusb_device_handle *handle;
    libusb_device_handle *accHandle;
    libusb_device *device;

};

#endif
