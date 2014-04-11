#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){

	// we don't want to be running to fast
    //ofSetVerticalSync(true);
    
    ofSetFrameRate(10);


    //tcpClient.setup(ADRESS, PORT);
	//optionally set the delimiter to something else.  The delimter in the client and the server have to be the same
	//tcpClient.setMessageDelimiter("\n");
	
	connectTime = 0;
	deltaTime = 0;
    rightWheelNeuron = leftWheelNeuron = 0.0;
    
	tcpClient.setVerbose(true);
    
    for(int i=0; i<CHANNEL_NUM; i++) {
        channelSpikedNum[i] = 0;
    }
    
    neuronEmbodied = false;
    led = 1;

    /*
	if (!usbdev.initUSB()) {
		cerr << "Problem connecting to the USB device." << endl;
        exit();
	}

	usbdev.setDebugLevel(3);
	elisa = new Elisa3Manager(&usbdev);
    elisaTestRunning = false;

    
    elisaIndex = elisa->addElisa3(ELISA_INDEX);
    
    stopElisa(elisaIndex);
    updateElisaNeuronEmbodied(elisaIndex);
     */
    
    //for calibration of some parameters
    gui.setup();
    gui.add(frameRate.setup( "frame rate", 10, 0, 100));
	gui.add(outputRateL.setup( "output rate Left", 0.3, 0, 1));
    gui.add(outputRateR.setup( "output rate Right", 0.3, 0, 1));
	gui.add(forwardSpeedL.setup( "left motor speed", 12.5, 0, 20));
    gui.add(forwardSpeedR.setup( "right motor speed", 12.5, 0, 20));
	gui.add(numberOfOutputNeurons.setup( "num of output neurons", 15, 0, 40));
    gui.add(IRThreshold.setup( "IR sensor threshold", 70, 0, 500));
    gui.add(MaxSpeed.setup( "max speed", 15, 0, 30));

    gui.setPosition(10, 10);
    
    setRecordSensorData();
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255);

    rightWheelNeuron = (double)forwardSpeedR;
    leftWheelNeuron = (double)forwardSpeedL;

	if(tcpClient.isConnected()){
        string data = tcpClient.receiveRaw();
        for(int i = 0; i < (int)data.size(); ++i){
            int c = (int)data[i];
            if(c<0 || c>=CHANNEL_NUM) {
                ofLog() << "Invalid Data: channel-" << (int)c;
            }
            channelSpikedNum[c]++;
            
            for(int j=0; j<(int)numberOfOutputNeurons; j++){
                if(c < 30 && leftWheelNeuron > -MaxSpeed) leftWheelNeuron -= (double)outputRateL;
                else if(c > 30 && rightWheelNeuron > -MaxSpeed) rightWheelNeuron -= (double)outputRateR;
            }
        }
        /*
        if (sendData[0] != -1) {
            tcpClient.sendRawBytes((char*)sendData, 2);
        }
         */
    }
    
    if (elisa != NULL) {


        if(irvalues[1] > IRThreshold && rand()%1000 <  irvalues[1]) {
            setStimulusData(0,inputR);
            if (tcpClient.isConnected()) {
                isSentStimulusData = sendStimulusData();
            }
            stimSign = true;
        }else if(irvalues[7] > IRThreshold && rand()%1000 <  irvalues[7]) {
            setStimulusData(1,inputL);
            if (tcpClient.isConnected()) {
                isSentStimulusData = sendStimulusData();
            }
            stimSign = true;
        }else {stimSign = false;}
        ofs << irvalues[0] << "," << irvalues[1] << "," << irvalues[7] << "\n";
 
        updateElisa(elisaIndex);
    }

}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetColor(0);
    
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
    
	ofDrawBitmapString(str, 250, 30);
    
    str = "";
    str = ofToString(leftWheelNeuron) + " / " + ofToString(rightWheelNeuron);
    ofDrawBitmapString(str, 500, 30);
    
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
    if (isSentStimulusData) {
        str += "DAC#";
        str += ofToString(stimulusDac);
        str += " Channel#";
        str += ofToString(stimulusChannel);
    }else{
        str += "None";
    }
    isSentStimulusData = false;
    ofDrawBitmapString(str, 15, 320);
    gui.draw();

}


//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'r': {
            if (!usbdev.initUSB()) {
                cerr << "Problem connecting to the USB device." << endl;
                exit();
            }
            
            usbdev.setDebugLevel(3);
            elisa = new Elisa3Manager(&usbdev);
            elisaIndex = elisa->addElisa3(ELISA_INDEX);
            elisaTestRunning = false;
            neuronEmbodied = false;
            led = 1;
            stopElisa(elisaIndex);
            //updateElisaNeuronEmbodied(elisaIndex);
            }
            break;
            
            
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
            
        case 'S': {
            setStimulusData(0, 3);
            isSentStimulusData = sendStimulusData();
            }
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
void testApp::setStimulusData(int dac, int channel){
    if( (dac != 0 && dac != 1) || channel < 0 || channel >125) {
        return;
    }
    stimulusDac = dac;
    stimulusChannel = channel;
}

//--------------------------------------------------------------
bool testApp::sendStimulusData(){
    unsigned char sendData = ((unsigned char)stimulusDac << 7) | (unsigned char)stimulusChannel;
    //char sendData = 0b10000011;
    std::cout << std::bitset<8>(sendData) << std::endl;
    tcpClient.sendRawBytes((char*)&sendData, 1);
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

void testApp::setRecordSensorData(){
    int year, month, day;
    int hour, minute;
    
    struct tm *date;
    time_t now;
    time(&now);
    date = localtime(&now);
    
    year = date->tm_year + 1900;
    month = date->tm_mon + 1;
    day = date->tm_mday;
    hour = date->tm_hour;
    minute = date->tm_min;
    
//    char dir[255];
//    getcwd(dir,255);
//    cout<<"Current Directory : "<<dir<<endl;
    
    std::stringstream layer_str;
    layer_str << "../../../sensordata"  <<"_" << year << "_" << month << "_" << day << "_" << hour << "_" << minute << "/";
    mkdir(layer_str.str().c_str(),S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    
    std::string result_C_path = layer_str.str() + "sensorData2.csv";
    ofs.open(result_C_path.c_str());
}
