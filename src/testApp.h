#pragma once

#include "ofMain.h"
#include "ofxNetwork.h"

#define CHANNEL_NUM 126

class testApp : public ofBaseApp{

public:

    void setup();
    void update();
	void draw();
		
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
		
	ofxTCPClient tcpClient;

    ofTrueTypeFont  mono;
	ofTrueTypeFont  monosm;

	float counter;
	int connectTime;
	int deltaTime;

    unsigned short int channelSpikedNum[CHANNEL_NUM];
    //bool weConnected;

	int size;
	int pos;
	bool typed;
    
private:
};

