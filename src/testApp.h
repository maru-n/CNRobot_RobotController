#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"
#include "UsbDevice.h"
#include "Elisa3Manager.h"

#define CHANNEL_NUM 126

//#define ELISA_INDEX 3259
#define ELISA_INDEX 3262

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
    
	ofxTCPClient tcpClient;

    ofTrueTypeFont  mono;
	ofTrueTypeFont  monosm;

	float counter;
	int connectTime;
	int deltaTime;

    unsigned short int channelSpikedNum[CHANNEL_NUM];
    //bool weConnected;
    
    float rightWheelNeuron, leftWheelNeuron;
    const float wheelSinapticWeight = 3.0;

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
};

