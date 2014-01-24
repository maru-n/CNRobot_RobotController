#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	// we don't want to be running to fast
    //ofSetVerticalSync(true);


    //tcpClient.setup(ADRESS, PORT);
	//optionally set the delimiter to something else.  The delimter in the client and the server have to be the same
	//tcpClient.setMessageDelimiter("\n");
	
	connectTime = 0;
	deltaTime = 0;

    rightWheelNeuron = leftWheelNeuron = 0.0;

	tcpClient.setVerbose(true);
    //stimulusTcpClient.setVerbose(true);

    for(int i=0; i<CHANNEL_NUM; i++) {
        channelSpikedNum[i] = 0;
    }

	if (!usbdev.initUSB()) {
		cerr << "Problem connecting to the USB device." << endl;
        exit();
	}

	usbdev.setDebugLevel(3);
	elisa = new Elisa3Manager(&usbdev);
    elisaTestRunning = false;
    neuronEmbodied = false;
    led = 1;
    
    elisaIndex = elisa->addElisa3(ELISA_INDEX);
    
    stopElisa(elisaIndex);
    updateElisaNeuronEmbodied(elisaIndex);
    
    sendData[0] = sendData[1] = -1;
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(230, 230, 230);

    if(irvalues[0]>50) {
        sendData[0] = 0; //DAC
        sendData[1] = 3; //channel
        tcpClient.sendRawBytes((char*)sendData, 2);
    }else{
        sendData[0] = -1;
        sendData[1] = -1;
    }

    rightWheelNeuron = leftWheelNeuron = 0.0;
	if(tcpClient.isConnected()){
        string data = tcpClient.receiveRaw();
        for(int i = 0; i < (int)data.size(); ++i){
            int c = (int)data[i];
            if(c<0 || c>=CHANNEL_NUM) {
                ofLog() << "Invalid Data: channel-" << (int)c;
            }
            channelSpikedNum[c]++;
            if (c<CHANNEL_NUM/2) {
                rightWheelNeuron += 1.0;
            }else{
                leftWheelNeuron += 1.0;
            }
        }

        if (sendData[0] != -1) {
            tcpClient.sendRawBytes((char*)sendData, 2);
        }
    }
    
    updateElisa(elisaIndex);
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
    
    str = "";
    str = ofToString(leftWheelNeuron) + " / " + ofToString(rightWheelNeuron);
    ofDrawBitmapString(str, 300, 30);
    
    str =  "IR values = " ;
    for (unsigned i=0; i<irvalues.size(); ++i) {
        str += "[";
        str += ofToString(i);
        str += "]";
        str += ofToString( irvalues[i] );
        str += "  ";
    }
    ofDrawBitmapString(str, 15, 300);
    
    str = "Stimulus: ";
    if (sendData[0] != -1) {
        str += "DAC#";
        str += ofToString(sendData[0]);
        str += " Channel#";
        str += ofToString(sendData[1]);
    }else{
        str += "None";
    }
    ofDrawBitmapString(str, 15, 320);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'c':
            if( !tcpClient.setup(ADRESS, PORT) ) {
                cerr << "Could not connect to signal server." << endl;
            }
            break;
            
        case 't':
            if (!elisaTestRunning) {
                startElisaTestRun(elisaIndex);
            }else{
                stopElisaTestRun(elisaIndex);
            }
            break;
            
        case 'e':
            stopElisaTestRun(elisaIndex);
            neuronEmbodied = !neuronEmbodied;
            break;
            
        case 's':
            stopElisa(elisaIndex);
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::exit(){
    stopElisa(elisaIndex);
    delete elisa;
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

//--------------------------------------------------------------
void testApp::updateElisa(int elisaIndex){
    if( elisaTestRunning ) {
        updateElisaTestRun(elisaIndex);
    }else if( neuronEmbodied ){
        updateElisaNeuronEmbodied(elisaIndex);
    }else{
        for (unsigned i=0; i<4; ++i) {
            if (!elisa->sendCommands())
                std::cout << "Data failed to be sent." << std::endl;
            if (elisa->receiveData())
                std::cout << "Data received." << std::endl;
        }
        elisa->getIRSensorValues(elisaIndex,irvalues);
    }
}

//--------------------------------------------------------------
void testApp::updateElisaNeuronEmbodied(int elisaIndex){
    elisa->setRGBLeds(elisaIndex, 256, 256, 256);
	elisa->setGreenLeds(elisaIndex, 0);
	elisa->setMotorSpeed(elisaIndex,
                         min((int)(leftWheelNeuron*wheelSinapticWeight),128),
                         min((int)(rightWheelNeuron*wheelSinapticWeight),128));
    for (unsigned i=0; i<4; ++i) {
        if (!elisa->sendCommands())
            std::cout << "Data failed to be sent." << std::endl;
        if (elisa->receiveData())
            std::cout << "Data received." << std::endl;
    }
    elisa->getIRSensorValues(elisaIndex,irvalues);
    elisaLastUpdateMillSec = ofGetElapsedTimeMillis();
}


//--------------------------------------------------------------
void testApp::stopElisa(int elisaIndex){
    elisa->setRGBLeds(elisaIndex, 0,0,0);
	elisa->setGreenLeds(elisaIndex, 0);
	elisa->setMotorSpeed(elisaIndex, 0, 0);
    for (unsigned i=0; i<4; ++i) {
        if (!elisa->sendCommands())
            std::cout << "Data failed to be sent." << std::endl;
        if (elisa->receiveData())
            std::cout << "Data received." << std::endl;
    }
    elisa->getIRSensorValues(elisaIndex,irvalues);
    elisaLastUpdateMillSec = ofGetElapsedTimeMillis();
    
    neuronEmbodied = false;
    elisaTestRunning = false;
}

//--------------------------------------------------------------
void testApp::startElisaTestRun(int elisaIndex){
    led = 1;
    elisaTestRunning = true;
}

//--------------------------------------------------------------
void testApp::stopElisaTestRun(int elisaIndex){
    stopElisa(elisaIndex);
    elisaTestRunning = false;
}

//--------------------------------------------------------------
void testApp::updateElisaTestRun(int elisaIndex){
    int nowSec = ofGetElapsedTimeMillis()/1000;
    int preSec = elisaLastUpdateMillSec/1000;
    if( nowSec != preSec ) {
        if (led == 128)
            led = 1;
        else
            led *=2;
        elisa->setGreenLeds(elisaIndex, led);
        elisa->setMotorSpeed(elisaIndex, 25, -25);
        for (unsigned i=0; i<4; ++i) {
            if (!elisa->sendCommands())
                std::cout << "Data failed to be sent." << std::endl;
            if (elisa->receiveData())
                std::cout << "Data received." << std::endl;
        }
        elisa->getIRSensorValues(elisaIndex,irvalues);
        elisaLastUpdateMillSec = ofGetElapsedTimeMillis();
    }
}
