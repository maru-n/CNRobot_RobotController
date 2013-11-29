#include "UsbDevice.h"
#include <iostream>

UsbDevice::UsbDevice()
{
	usbcontext = NULL;
	devlist = NULL;
	devhandle = NULL;
}

UsbDevice::~UsbDevice()
{
	closeUSBDevice();

	if (usbcontext) {

		// libusb_free_device_list(devlist,1);
		libusb_exit(usbcontext);

	}
}

bool UsbDevice::initUSB()
{
	int error;
	error=libusb_init(&usbcontext);
	if (error != 0) {
		std::cerr << "UsbDevice : Error during the initialization of the library : " << libusb_error_name(error) << std::endl;
		return false;
	}

	// nbdevices = libusb_get_device_list(usbcontext, &devlist);

	return acquireUSBDevice();
}

bool UsbDevice::acquireUSBDevice()
{
	devhandle = (libusb_device_handle*)libusb_open_device_with_vid_pid(usbcontext, 0x1915, 0x0101);
	// devhandle = (libusb_device_handle*)libusb_open_device_with_vid_pid(usbcontext, 0x0403, 0x6001);
	if (!devhandle) {
		std::cerr << "UsbDevice : Error during the acquisition of the the interface: Failure to find the device." << std::endl;
		return false;
	}

	int error = libusb_claim_interface(devhandle, 0);
	if (error!=0) {
		std::cerr << "UsbDevice : Error during the acquisition of the interface : " << libusb_error_name(error) << std::endl;
		return false;
	}

	return devhandle != NULL;
}

void UsbDevice::closeUSBDevice()
{
	if (devhandle) {
		libusb_release_interface(devhandle,0);
		libusb_close(devhandle);
		devhandle = NULL;
	}
}


bool UsbDevice::send(unsigned char* data, int size)
{
	int transferred = 0;
	int error = libusb_bulk_transfer(devhandle, 0x01, data, 64, &transferred, 5000);
	if (error<0) {
		std::cerr << "UsbDevice : Error when sending the data : " << libusb_error_name (error) << std::endl;
		return false;
	}
	if (transferred < size) {
		std::cerr << "UsbDevice : Error when sending the data : not enough data sent." << std::endl;
		return false;
	}
	return true;
}

int UsbDevice::receive(unsigned char* data, int size)
{
	int received = 0;
	int error = libusb_bulk_transfer(devhandle, 0x81, data, size, &received, 5000);
	if (error) {
		std::cerr << "UsbDevice : Error when receiving data : " << libusb_error_name (error) << std::endl;
		return 0;
	}

	if (received < size) {
		std::cerr << "UsbDevice : Error when receiving data : Not enought data received : " << received << std::endl;
	}
	return received;
}

void UsbDevice::setDebugLevel(int level)
{
	libusb_set_debug(usbcontext, level);
}

void UsbDevice::printEndPoints()
{	
	std::cout << "Printing endpoints for the current device." << std::endl;

	libusb_config_descriptor* cfgdes;
	libusb_device* dev = libusb_get_device(devhandle); 
	const libusb_interface_descriptor* idesc;


	int error = libusb_get_active_config_descriptor(dev, &cfgdes);
	if (error) {
		std::cerr << "UsbDevice : Error when obtaining the config descriptor : " << libusb_error_name(error) << std::endl;
		return;
	}
	else {
		for (unsigned i=0; i<cfgdes->bNumInterfaces; ++i) {
			std::cout << "Interface " << i << std::endl;
			for (int s=0; s< cfgdes->interface[i].num_altsetting; ++s) {
				std::cout << "  setting " << s << std::endl;
				idesc = &(cfgdes->interface[i].altsetting[s]);
				std::cout << "     number of endpoints = " << (int)idesc->bNumEndpoints << std::endl;
				for (unsigned e=0; e< idesc->bNumEndpoints ; ++e) {
					uint8_t endpoint = idesc->endpoint[e].bEndpointAddress;
					std::cout << "    endpoint " << e << " : type = " << (endpoint << 7 == LIBUSB_ENDPOINT_IN ? "IN" : "OUT") << " ; address = " << (int)endpoint << std::endl;
				}
			}
		}
	}

}




