#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {

	ofSetLogLevel(ofxOpenNIContext::LOG_NAME, OF_LOG_NOTICE);
    ofSetLogLevel(openNIDevice.LOG_NAME, OF_LOG_NOTICE);
    
    openNIDevice.setup();
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();   // comment this out
    //openNIDevice.addInfraGenerator(); // and uncomment this to see infrared generator
    
}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    
	ofSetColor(255, 255, 255);
    
    openNIDevice.drawDepth(0, 0);
    openNIDevice.drawImage(640, 0);
    
	ofSetColor(0, 255, 0);
	string msg = "FPS: " + ofToString(ofGetFrameRate());
	ofDrawBitmapString(msg, 20, ofGetHeight() - 20);
    
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

