#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "UsbDevice.h"
#include "Elisa3Manager.h"


#define RECONNECT_TIME 400

#define ADRESS "127.0.0.1"
#define PORT 3030
//#define STIMULUS_PORT 32131

#define CHANNEL_NUM 126

//#define ELISA_INDEX 3259
#define ELISA_INDEX 3261

#define IR_THRESHOLD 20

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
    int outputL[40] = {38 , 10 , 97 , 99 , 85 , 80 , 91 , 83 , 94 , 110 , 5 , 59 , 82 , 124 , 42 , 107 , 96 , 4 , 79 , 16 , 100 , 18 , 61 , 6 , 81 , 119 , 55 , 14 , 89 , 78 , 103 , 76 , 17 , 98 , 101 , 22 , 90 , 104 , 25 , 75};
    int outputR[40] = {3 , 13 , 122 , 86 , 44 , 108 , 92 , 111 , 95 , 62 , 8 , 11 , 45 , 105 , 90 , 1 , 123 , 20 , 69 , 88 , 19 , 89 , 103 , 15 , 23 , 117 , 77 , 87 , 55 , 6 , 114 , 73 , 25 , 17 , 118 , 107 , 50 , 7 , 26 , 4};
    int inputL = 47;
    int inputR = 21;
    
    bool stimSign;
};

