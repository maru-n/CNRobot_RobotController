#include "Elisa3Manager.h"
#include "UsbDevice.h"
#include <iostream>


Elisa3Manager::Elisa3Manager(UsbDevice* _usbdev)
	: usbdev(_usbdev)
{
	elisaList.reserve(4);
	elisaSensors.reserve(4);
	robotData.reserve(4);
}

Elisa3Manager::~Elisa3Manager()
{
	// delete usbdev;
}

int Elisa3Manager::addElisa3(unsigned id)
{
	elisaList.push_back(id);
	elisaSensors.push_back(ALLSENSORS);
	robotData.push_back(RobotData());
	unsigned index = elisaList.size()-1;
	TXBuffer[index*ROBOTPACKETSIZE+13+1] = (id>>8) & 0xFF;
	TXBuffer[index*ROBOTPACKETSIZE+14+1] = id & 0xFF;

	return index;
}

void Elisa3Manager::resetTXBuffer()
{
	bzero(TXBuffer, sizeof(unsigned char)*PACKETSIZE);
	TXBuffer[0] = 0x27;

	for (unsigned i=0; i<elisaList.size(); ++i) {
		TXBuffer[i*ROBOTPACKETSIZE+13+1] = (elisaList[i]>>8) & 0xFF;
		TXBuffer[i*ROBOTPACKETSIZE+14+1] = elisaList[i] & 0xFF;
	}
}

void Elisa3Manager::testElisa3(uint16_t id)
{
	if (usbdev) 
	{
		unsigned char buffer[64];
		bzero(buffer, 64*sizeof(unsigned char));
		unsigned char* ptr;

		ptr = reinterpret_cast<unsigned char*>(&id);
		buffer[0] = 0x27;
		buffer[2] = ptr[0];
		buffer[1] = ptr[1];
		usbdev->send(buffer,3);


	}
}

char speed(char value) {
    if(value >= 0) {
        return (value|0x80);
    } else {
        return ((-value)&0x7F);
    }
}

void Elisa3Manager::setMotorSpeed(int robot, char left, char right)
{
	unsigned index = robot * ROBOTPACKETSIZE + 1 + 4;
	TXBuffer[index] = speed(right);
	TXBuffer[index+1] = speed(left);
	// TXBuffer[index] = right;
	// TXBuffer[index+1] = left;
}

void Elisa3Manager::setGreenLeds(int robot, unsigned char leds)
{
	unsigned index = robot * ROBOTPACKETSIZE + 1 + 6;
	TXBuffer[index] = leds;
}

void Elisa3Manager::setRGBLeds(int robot, unsigned char R, unsigned char G, unsigned char B)
{
	unsigned index = robot * ROBOTPACKETSIZE + 1;

	TXBuffer[index] = R;
	TXBuffer[index+1] = B;
	TXBuffer[index+2] = G;
}

void Elisa3Manager::calibrateSensors(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 +1;

	if (onoff)
		TXBuffer[index] |= 1<<4;
	else
		TXBuffer[index] &= ~(1<<4);
}

void Elisa3Manager::activateIRRemoteControl(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 + 1;

	if (onoff)
		TXBuffer[index] |= 1<<2;
	else
		TXBuffer[index] &= ~(1<<2);
}

void Elisa3Manager::activateWallAvoidance(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 + 1;

	if (onoff)
		TXBuffer[index] |= 1<<6;
	else
		TXBuffer[index] &= ~(1<<6);
}

void Elisa3Manager::activateCliffAvoidance(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 + 1;

	if (onoff)
		TXBuffer[index] |= 1<<7;
	else
		TXBuffer[index] &= ~(1<<7);
}

void Elisa3Manager::activateIRSensors(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 + 1;

	if (onoff)
		TXBuffer[index] |= 1<<0 | 1<<1;
	else
		TXBuffer[index] &= ~(1<<0 | 1<<1);

}

void Elisa3Manager::activateFrontIRSensors(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 + 1;

	if (onoff)
		TXBuffer[index] |= 1<<1;
	else
		TXBuffer[index] &= ~(1<<1);

}

void Elisa3Manager::activateBackIRSensors(int robot, bool onoff)
{
	unsigned index = robot * ROBOTPACKETSIZE + 3 + 1;

	if (onoff)
		TXBuffer[index] |= 1<<0;
	else
		TXBuffer[index] &= ~(1<<0);
}

void Elisa3Manager::activateSensors(int robot, unsigned char sensors)
{
	unsigned index = robot*ROBOTPACKETSIZE + 1 + 8;

	TXBuffer[index] = sensors;
	elisaSensors[robot] = sensors;
}


void Elisa3Manager::getIRSensorValues(int robot, std::vector<unsigned>& irvalues)
{
	irvalues = robotData[robot].irvalues;
}

void Elisa3Manager::getAccelerometer(int robot, int& x, int& y, int& z)
{
	x = robotData[robot].accelX;
	y = robotData[robot].accelY;
	z = robotData[robot].accelZ;
}

void Elisa3Manager::getBatteryLevel(int robot, unsigned& level, unsigned& percent)
{
	level = robotData[robot].batteryLevel;
	percent = 0;
}

void Elisa3Manager::getAmbientSensorValues(int robot, std::vector<unsigned>& ambientvalues)
{
	ambientvalues = robotData[robot].ambientvalues;
}

void Elisa3Manager::getFloorSensorValues(int robot, std::vector<unsigned>& floorvalues)
{
	floorvalues = robotData[robot].floorvalues;
}

void Elisa3Manager::getFloorSensorAmbientValues(int robot, std::vector<unsigned>& ambientfloorvalues)
{
	ambientfloorvalues = robotData[robot].ambientfloorvalues;	
}


bool Elisa3Manager::sendCommands()
{
	unsigned size = elisaList.size()*ROBOTPACKETSIZE + 1;

	TXBuffer[0] = 0x27;

	std::cout << "Data to send : ";
	for (unsigned i=0; i<size; ++i)
		std::cout << (int)TXBuffer[i] << " ";
	std::cout << std::endl;

	return usbdev->send(reinterpret_cast<unsigned char*>(TXBuffer), size);
}

bool Elisa3Manager::receiveData()
{
	bzero(RXBuffer, PACKETSIZE*sizeof(unsigned char));

	int received = usbdev->receive(reinterpret_cast<unsigned char*>(RXBuffer), PACKETSIZE);

    if (received < PACKETSIZE) {
        std::cout << "An error occured while receiving some data. Data received = " << received << std::endl;
        return false;
    }
    else {
    	unsigned robotid = 0;
    	unsigned char packetid = RXBuffer[0];
    	switch(packetid) {
    		case 0:
    			std::cout << "Transmission success." << std::endl;
    			break;
			case 1:
				std::cout << "Ack received." << std::endl;
				break;
			case 2:
				std::cout << "Transmission failed." << std::endl;
				break;
    		case 3:
    			robotData[robotid].irvalues[0] = ((int)RXBuffer[2]<<8) | ((unsigned)RXBuffer[1]);
    			robotData[robotid].irvalues[1] = ((int)RXBuffer[4]<<8) | ((unsigned)RXBuffer[3]);
    			robotData[robotid].irvalues[2] = ((int)RXBuffer[6]<<8) | ((unsigned)RXBuffer[5]);
    			robotData[robotid].irvalues[3] = ((int)RXBuffer[8]<<8) | ((unsigned)RXBuffer[7]);
    			robotData[robotid].irvalues[5] = ((int)RXBuffer[10]<<8) | ((unsigned)RXBuffer[9]);
    			robotData[robotid].irvalues[6] = ((int)RXBuffer[12]<<8) | ((unsigned)RXBuffer[11]);
    			robotData[robotid].irvalues[7] = ((int)RXBuffer[14]<<8) | ((unsigned)RXBuffer[13]);
    			break;
    		case 4:
    			robotData[robotid].irvalues[4] = ((int)RXBuffer[2]<<8) | ((unsigned)RXBuffer[1]);
				robotData[robotid].floorvalues[0] = ((int)RXBuffer[4]<<8) | ((unsigned)RXBuffer[3]);
				robotData[robotid].floorvalues[1] = ((int)RXBuffer[6]<<8) | ((unsigned)RXBuffer[5]);
				robotData[robotid].floorvalues[2] = ((int)RXBuffer[8]<<8) | ((unsigned)RXBuffer[7]);
				robotData[robotid].floorvalues[3] = ((int)RXBuffer[10]<<8) | ((unsigned)RXBuffer[9]);
				robotData[robotid].accelX = ((int)RXBuffer[12]<<8) | ((unsigned)RXBuffer[11]);
				robotData[robotid].accelY = ((int)RXBuffer[14]<<8) | ((unsigned)RXBuffer[13]);
    			break;
			case 5:
    			robotData[robotid].ambientvalues[0] = ((int)RXBuffer[2]<<8) | ((unsigned)RXBuffer[1]);
    			robotData[robotid].ambientvalues[1] = ((int)RXBuffer[4]<<8) | ((unsigned)RXBuffer[3]);
    			robotData[robotid].ambientvalues[2] = ((int)RXBuffer[6]<<8) | ((unsigned)RXBuffer[5]);
    			robotData[robotid].ambientvalues[3] = ((int)RXBuffer[8]<<8) | ((unsigned)RXBuffer[7]);
    			robotData[robotid].ambientvalues[5] = ((int)RXBuffer[10]<<8) | ((unsigned)RXBuffer[9]);
    			robotData[robotid].ambientvalues[6] = ((int)RXBuffer[12]<<8) | ((unsigned)RXBuffer[11]);
    			robotData[robotid].ambientvalues[7] = ((int)RXBuffer[14]<<8) | ((unsigned)RXBuffer[13]);
				break;
			case 6:
    			robotData[robotid].ambientvalues[4] = ((int)RXBuffer[2]<<8) | ((unsigned)RXBuffer[1]);
    			robotData[robotid].ambientfloorvalues[0] = ((int)RXBuffer[4]<<8) | ((unsigned)RXBuffer[3]);
				robotData[robotid].ambientfloorvalues[1] = ((int)RXBuffer[6]<<8) | ((unsigned)RXBuffer[5]);
				robotData[robotid].ambientfloorvalues[2] = ((int)RXBuffer[8]<<8) | ((unsigned)RXBuffer[7]);
				robotData[robotid].ambientfloorvalues[3] = ((int)RXBuffer[10]<<8) | ((unsigned)RXBuffer[9]);
				robotData[robotid].accelZ = ((int)RXBuffer[12]<<8) | ((unsigned)RXBuffer[11]);
				robotData[robotid].batteryLevel = ((int)RXBuffer[14]<<8) | ((unsigned)RXBuffer[13]);
				break;
			case 7:
				break;
			default:
				std::cout << "Uknown packet received." << std::endl;
				break;
    	}


        return true;
    }
}


