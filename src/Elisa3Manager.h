#ifndef __ELISA3_MANAGER__
#define __ELISE3_MANAGER__

#include <vector>

class UsbDevice;

#define PACKETSIZE 64
#define PAYLOADSIZE 13
#define ROBOTPACKETSIZE (PAYLOADSIZE+2)

class Elisa3Manager
{
public:
	enum SENSORS{ALLSENSORS=255, IR=1, ACCEL=2, BATTERY=4, AMBIENTIR=8, FLOOR=16, AMBIENTFLOOR=32};
private:
	UsbDevice* usbdev;
	std::vector<unsigned> elisaList;

	unsigned char TXBuffer[PACKETSIZE];
	unsigned char RXBuffer[PACKETSIZE];

	std::vector<char*> elisaStates;
	std::vector<unsigned char> elisaSensors;

	class RobotData {
	public:
		std::vector<unsigned> irvalues;
		std::vector<unsigned> ambientvalues;
		std::vector<unsigned> floorvalues;
		std::vector<unsigned> ambientfloorvalues;
		int accelX, accelY, accelZ;
		unsigned batteryLevel, batteryPercent;

		RobotData() {
			irvalues.resize(8,0);
			ambientvalues.resize(8,0);
			floorvalues.resize(4,0);
			ambientfloorvalues.resize(4,0);
			accelX = 0;
			accelY = 0;
			accelZ = 0;
			batteryLevel = 0;
			batteryPercent = 0;
		}

	};

	typedef class RobotData robotdata_t;

	std::vector<robotdata_t> robotData; 

public:
	Elisa3Manager(UsbDevice* _usbdev=NULL);
	virtual ~Elisa3Manager();

	int addElisa3(unsigned id);

	void resetTXBuffer();
// Elisa3 control
	void testElisa3(uint16_t id);


	/**** SETTTING THE ROBOT COMMANDS ***************************************************/
	void setMotorSpeed(int robot, char left, char right);
	void setGreenLeds(int robot, unsigned char leds);
	void setRGBLeds(int robot, unsigned char R, unsigned char G, unsigned char B);
	void calibrateSensors(int robot, bool onoff);
	void activateIRRemoteControl(int robot, bool onoff);
	void activateWallAvoidance(int robot, bool onoff);
	void activateCliffAvoidance(int robot, bool onoff);

	void activateIRSensors(int robot, bool onoff);
	void activateFrontIRSensors(int robot, bool onoff);
	void activateBackIRSensors(int robot, bool onoff);

	void activateSensors(int robot, unsigned char sensors);

	/**** RECEIVED SENSOR READINGS ***************************************************/
	void getIRSensorValues(int robot, std::vector<unsigned>& irvalues);
	void getAccelerometer(int robot, int& x, int& y, int& z);
	void getBatteryLevel(int robot, unsigned& level, unsigned& percent);
	void getAmbientSensorValues(int robot, std::vector<unsigned>& ambientvalues);
	void getFloorSensorValues(int robot, std::vector<unsigned>& floorvalues);
	void getFloorSensorAmbientValues(int robot, std::vector<unsigned>& ambientfloorvalues);



	/**** UPDATING THE ROBOT STATUS ***************************************************/
	bool sendCommands();
	bool receiveData();





};

#endif

