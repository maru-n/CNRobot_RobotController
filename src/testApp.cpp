#include "testApp.h"

#define RECONNECT_TIME 400

#define ADRESS "192.168.56.3"
#define PORT 12345

//--------------------------------------------------------------
void testApp::setup(){

	// we don't want to be running to fast
    //ofSetVerticalSync(true);


    //tcpClient.setup(ADRESS, PORT);
	//optionally set the delimiter to something else.  The delimter in the client and the server have to be the same
	//tcpClient.setMessageDelimiter("\n");
	
	connectTime = 0;
	deltaTime = 0;

	tcpClient.setVerbose(true);
    
    for(int i=0; i<CHANNEL_NUM; i++) {
        channelSpikedNum[i] = 0;
    }

}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(230, 230, 230);

	if(tcpClient.isConnected()){
        string data = tcpClient.receiveRaw();
        for(int i = 0; i < (int)data.size(); ++i){
            int c = (int)data[i];
            if(c<0 || c>=CHANNEL_NUM) {
                ofLog() << "Invalid Data: channel-" << (int)c;
            }
            channelSpikedNum[c]++;
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(20, 20, 20);
    
    string str = "";
    for (int i=0; i<CHANNEL_NUM; i++) {
        str+=ofToString(channelSpikedNum[i]);
        if(i%10 == 9) {
            str+="\n";
        }else{
            str+=" ";
        }
        channelSpikedNum[i] = 0;
    }
    
	ofDrawBitmapString(str, 15, 30);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'r':
            tcpClient.setup(ADRESS, PORT);
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}
