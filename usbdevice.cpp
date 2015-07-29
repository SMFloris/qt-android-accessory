#include "usbdevice.h"

UsbDevice::UsbDevice()
{
    handle = nullptr;
}

UsbDevice::~UsbDevice()
{

}

void UsbDevice::setDeviceHandle(libusb_device_handle* devHandle)
{
    handle = devHandle;
}

void UsbDevice::setProductId(int pId)
{
    productId = pId;
}

void UsbDevice::setVendorId(int vId)
{
    vendorId = vId;
}

int UsbDevice::pId()
{
    return productId;
}

int UsbDevice::vId()
{
    return vendorId;
}

libusb_device_handle* UsbDevice::getHandle()
{
    return handle;
}

libusb_device_handle* UsbDevice::getAccHandle()
{
    return accHandle;
}

void UsbDevice::setDev(libusb_device* dev)
{
    device = dev;
}

void UsbDevice::setDevAccHandle(libusb_device_handle* devHandle)
{
    accHandle = devHandle;
}

libusb_device* UsbDevice::getDev()
{
    return device;
}





