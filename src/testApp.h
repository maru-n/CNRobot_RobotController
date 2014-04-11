#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "UsbDevice.h"
#include "Elisa3Manager.h"
#include "ofxGui.h"

#include <fstream>
#include <time.h>
#include <sys/stat.h>
#include <unistd.h>


#define RECONNECT_TIME 400

#define ADRESS "127.0.0.1"
#define PORT 3030
//#define STIMULUS_PORT 32131

#define CHANNEL_NUM 126

//#define ELISA_INDEX 3259
#define ELISA_INDEX 3261

//#define IR_THRESHOLD 20

class testApp : public ofBaseApp{

public:

    void setup();
    void update();
	void draw();
    void exit();
		
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:
    void stopElisa(int elisaIndex);
    void startElisaTestRun(int elisaIndex);
    void stopElisaTestRun(int elisaIndex);
    void updateElisa(int elisaIndex);
    void updateElisaTestRun(int elisaIndex);
    void updateElisaNeuronEmbodied(int elisaIndex);
    void setStimulusData(int dac, int channel);
    bool sendStimulusData();
    void setRecordSensorData();

    //void sendStimulus(unsigne char channel);
    
	ofxTCPClient tcpClient;

    ofTrueTypeFont  mono;
	ofTrueTypeFont  monosm;

	float counter;
	int connectTime;
	int deltaTime;

    unsigned short int channelSpikedNum[CHANNEL_NUM];
    
    float rightWheelNeuron, leftWheelNeuron;
    const float wheelSinapticWeight = 2.0;

	int size;
	int pos;
	bool typed;
    
    UsbDevice usbdev;
    Elisa3Manager* elisa;
    
    bool elisaTestRunning;
    bool neuronEmbodied;
    
    unsigned long long elisaLastUpdateMillSec;
    int elisaIndex;
    unsigned char led=1;
    std::vector<unsigned> irvalues;
    
    bool isSentStimulusData;
    int stimulusDac;
    int stimulusChannel;
    int count = 0;
    int outputL[40] = {12 , 91 , 123 , 102 , 62 , 99 , 77 , 10 , 88 , 80 , 78 , 117 , 90 , 14 , 89 , 108 , 95 , 41 , 79 , 24 , 9 , 6 , 98 , 74 , 47 , 83 , 111 , 86 , 16 , 56 , 21 , 71 , 0 , 68 , 19 , 120 , 18 , 27 , 96 , 105};
    int outputR[40] = {92 , 30 , 27 , 38 , 63 , 98 , 41 , 89 , 86 , 14 , 90 , 117 , 96 , 10 , 95 , 47 , 108 , 74 , 71 , 6 , 19 , 88 , 91 , 21 , 15 , 65 , 78 , 123 , 12 , 97 , 0 , 9 , 102 , 77 , 18 , 44 , 62 , 114 , 99 , 80};
    int inputL = 15;
    int inputR = 11;
    
    bool stimSign;
    
    ofxPanel gui;
    ofxFloatSlider outputRateL,outputRateR, forwardSpeedL, forwardSpeedR;
    ofxIntSlider frameRate, numberOfOutputNeurons, IRThreshold, MaxSpeed;
    
    std::ofstream ofs;
};

