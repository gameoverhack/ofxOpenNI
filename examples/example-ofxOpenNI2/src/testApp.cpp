#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
    device.setup();
    device.addDepthStream();
    device.addImageStream();
    device.addHandsTracker();
    device.addUserTracker();
    device.start();
    
    ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void testApp::update(){
    device.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    
    device.draw();
    
    ostringstream os;
    os << "FPS: " << ofGetFrameRate();
    ofDrawBitmapString(os.str(), 20, ofGetHeight() - 20);
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

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}