#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofSetLogLevel(ofxOpenNI::LOG_NAME, OF_LOG_VERBOSE);
	ofSetLogLevel(ofxOpenNIContext::LOG_NAME, OF_LOG_VERBOSE);

	openNI[0].setup();
	openNI[0].addDepthGenerator();
	openNI[0].addImageGenerator();
	openNI[1].setup();
	openNI[1].addDepthGenerator();
	openNI[1].addImageGenerator();
}

//--------------------------------------------------------------
void testApp::update(){
	openNI[0].update();
	openNI[1].update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
	openNI[0].drawDepth(0, 0);
	openNI[0].drawImage(640, 0);
	openNI[1].drawDepth(0, 480);
	openNI[1].drawImage(640, 480);
	
	ofSetColor(0, 255, 0);
	string msg = "FPS: " + ofToString(ofGetFrameRate()) + " MICROS: " + ofToString(ofGetElapsedTimeMicros());
	ofDrawBitmapString(msg, 20, ofGetHeight() - 20);
}


//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

string testApp::generateFileName() {

//	string _root = "kinectRecord";
//
//	string _timestamp = ofToString(ofGetDay()) +
//	ofToString(ofGetMonth()) +
//	ofToString(ofGetYear()) +
//	ofToString(ofGetHours()) +
//	ofToString(ofGetMinutes()) +
//	ofToString(ofGetSeconds());
//
//	string _filename = (_root + _timestamp + ".oni");
//
//	return _filename;

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

