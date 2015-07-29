#include "usbserver.h"

#include <QDebug>
#include <QString>
#include "usbdevice.h"

#define IN 0x85
#define OUT 0x07

static int attach_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);
static int detach_callback(libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data);
static void receive_callback(libusb_transfer *transfer);

UsbServer::UsbServer()
{
    info.manufacturer = QString("KDE");
    info.model = QString("KDE Connect");
    info.description = QString("KDE Connect USB Connection");
    info.versionName = QString("KCUsb - 0.1");
    info.url = QString("http://github.com/grgvineet/kdeconnect-kde");
    info.serial = QString("02101994SerialNo.");
}

UsbServer::~UsbServer()
{

}

void UsbServer::receive(unsigned char* msg, unsigned int size)
{
    emit onReceived(msg,size);
}

static void receive_callback(libusb_transfer* transfer)
{

}

static int attach_callback (libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data)
{
    qDebug()<<"Device attached!";
    UsbServer *server = (UsbServer*) user_data;
    server->attach(dev);

    return 0;
}

void UsbServer::attach(libusb_device* dev)
{
    struct libusb_device_descriptor desc;
    int rc;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
        qWarning()<< "Error getting device descriptor";
    }

    printf ("Device attach: %04x:%04x\n", desc.idVendor, desc.idProduct);

    libusb_device_handle *handle;
    rc = libusb_open (dev, &handle);
    if(rc){
        //qWarning()<<"ERROR OPENING DEVICE";
        return;
    }
    handles[QPair<uint16_t, uint16_t>(desc.idVendor, desc.idProduct)] = handle;

    rc = libusb_claim_interface(handles[QPair<uint16_t, uint16_t>(desc.idVendor, desc.idProduct)], 0);
    if (rc) {
        qWarning()<<"Could not claim interface!";
        return;
    }
    accessory(dev);
}

int UsbServer::accessory(libusb_device *dev)
{
    struct libusb_device_descriptor desc;
    int rc;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
        qWarning()<< "Error getting device descriptor";
    }
    libusb_device_handle *handle = handles[QPair<uint16_t, uint16_t>(desc.idVendor, desc.idProduct)];

    unsigned char ioBuffer[2];
    int devVersion;
    int response;
    int tries = 5;

    response = libusb_control_transfer(
        handle, //handle
        0xC0, //bmRequestType
        51, //bRequest
        0, //wValue
        0, //wIndex
        ioBuffer, //data
        2, //wLength
        0 //timeout
    );

    if(response < 0){qDebug()<<libusb_error_name(response);return-1;}

    devVersion = ioBuffer[1] << 8 | ioBuffer[0];
    qDebug()<<"Version Device Code: "<< devVersion;

    //wait a bit, some devices are a bit slower on their end
    usleep(1000);

    response = libusb_control_transfer(handle,0x40,52,0,0,(unsigned char*)info.manufacturer.toUtf8().constData(),info.manufacturer.size()+1,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}
    response = libusb_control_transfer(handle,0x40,52,0,1,(unsigned char*)info.model.toUtf8().constData(),info.model.size()+1,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}
    response = libusb_control_transfer(handle,0x40,52,0,2,(unsigned char*)info.description.toUtf8().constData(),info.description.size()+1,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}
    response = libusb_control_transfer(handle,0x40,52,0,3,(unsigned char*)info.versionName.toUtf8().constData(),info.versionName.size()+1,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}
    response = libusb_control_transfer(handle,0x40,52,0,4,(unsigned char*)info.url.toUtf8().constData(),info.url.size()+1,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}
    response = libusb_control_transfer(handle,0x40,52,0,5,(unsigned char*)info.serial.toUtf8().constData(),info.serial.size()+1,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}

    qDebug()<<"Accessory Identification sent";

    response = libusb_control_transfer(handle,0x40,53,0,0,NULL,0,0);
    if(response < 0){qDebug()<<libusb_error_name(response);return -1;}

    qDebug()<<"Attempted to put device into accessory mode";

    if(handle != NULL)
        libusb_release_interface (handle, 0);

    //connect to accessory mode new pid
    for(;;){
        tries--;
        if((handle = libusb_open_device_with_vid_pid(NULL, desc.idVendor, 0x2D01)) == NULL){
            if(tries < 0){
                return -1;
            }
        }else{
            break;
        }
        sleep(1);
    }
    libusb_claim_interface(handle, 0);
    qDebug()<<"Interface claimed, ready to transfer data";
}


static int detach_callback (libusb_context *ctx, libusb_device *dev, libusb_hotplug_event event, void *user_data) {
    qDebug()<<"Device detached\n";

    UsbServer *server = (UsbServer*) user_data;
    server->detach(dev);


    return 0; //rearm callback
}

void UsbServer::detach(libusb_device* dev)
{
    struct libusb_device_descriptor desc;
    int rc;

    rc = libusb_get_device_descriptor(dev, &desc);
    if (LIBUSB_SUCCESS != rc) {
        fprintf (stderr, "Error getting device descriptor\n");
    }

    printf ("Device detach: %04x:%04x\n", desc.idVendor, desc.idProduct);

    libusb_close (handles[QPair<uint16_t, uint16_t>(desc.idVendor, desc.idProduct)]);
}

int UsbServer::init()
{
    libusb_hotplug_callback_handle hp[2];
    int product_id, vendor_id, class_id;
    int rc;

    vendor_id  = -1;
    product_id = -1;
    class_id   = LIBUSB_HOTPLUG_MATCH_ANY;

    libusb_init (NULL);

    if (!libusb_has_capability (LIBUSB_CAP_HAS_HOTPLUG)) {
        printf ("Hotplug not supported by this build of libusb\n");
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_ARRIVED, LIBUSB_HOTPLUG_ENUMERATE, vendor_id,
                                           product_id, class_id, attach_callback, this, &hp[0]);
    if (LIBUSB_SUCCESS != rc) {
        fprintf (stderr, "Error registering callback 0\n");
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    rc = libusb_hotplug_register_callback (NULL, LIBUSB_HOTPLUG_EVENT_DEVICE_LEFT, LIBUSB_HOTPLUG_ENUMERATE, vendor_id,
                                           product_id,class_id, detach_callback, this, &hp[1]);
    if (LIBUSB_SUCCESS != rc) {
        fprintf (stderr, "Error registering callback 1\n");
        libusb_exit (NULL);
        return EXIT_FAILURE;
    }

    checkTimeout.setSingleShot(false);
    checkTimeout.setInterval(0);
    connect(&checkTimeout, SIGNAL(timeout()),this,SLOT(handleEvents()));

    checkTimeout.start();


}

void UsbServer::handleEvents()
{
    libusb_handle_events(NULL);
}

void UsbServer::stop()
{
    libusb_exit (NULL);
}

void UsbServer::send(unsigned char* msg, unsigned int size, UsbDevice* dest)
{

}