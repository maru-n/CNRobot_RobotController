#ifndef __USB_DEVICE__
#define __USB_DEVICE__

#include <libusb-1.0/libusb.h>

class UsbDevice
{
private:
	int device_id;

	libusb_device_handle* devhandle;
	libusb_context* usbcontext;
	libusb_device** devlist;
	ssize_t nbdevices;

public:
	UsbDevice();
	virtual ~UsbDevice();

	bool initUSB();

	bool acquireUSBDevice();
	void closeUSBDevice();

	bool send(unsigned char* data, int size);
	int receive(unsigned char* data, int size);

	void setDebugLevel(int level);

	void printEndPoints();
};


#endif

