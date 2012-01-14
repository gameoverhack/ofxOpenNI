#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {

    numDevices = openNIDevices[0].getNumDevices();
    
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        ofSetLogLevel(openNIDevices[deviceID].LOG_NAME, OF_LOG_NOTICE);
        openNIDevices[deviceID].setup();
        openNIDevices[deviceID].addDepthGenerator();
        openNIDevices[deviceID].addImageGenerator();
    }
    
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

//--------------------------------------------------------------
void testApp::update(){
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].update();
    }
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    
    ofPushMatrix();
    
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        ofTranslate(0, deviceID * 480);
        openNIDevices[deviceID].drawDepth(0, 0);
        openNIDevices[deviceID].drawImage(640, 0);
    }
	
    ofPopMatrix();
    
	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
	verdana.drawString(msg, 20, numDevices * 480 - 20);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

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

