#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    ofSetLogLevel(OF_LOG_NOTICE);

    openNIDevice.setup();
    //openNIDevice.setLogLevel(OF_LOG_VERBOSE);
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();
    openNIDevice.addUserGenerator();
    
    openNIDevice.setRegister(true); // register depth to image
    openNIDevice.setMirror(true);
    
    openNIDevice.setMaxNumUsers(2); // defualt is 4
    openNIDevice.setUseMaskTextureAllUsers(true);
    openNIDevice.setUsePointCloudsAllUsers(true);
    openNIDevice.setPointCloudDrawSizeAllUsers(3); // sets the size the points are drawn on screen (in pixels)
    openNIDevice.setPointCloudResolutionAllUsers(2); // sets the step between pixels and point cloud eg., 1 is every pixel, 2 is every second pixel etc

    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(0, 0, 0);
    openNIDevice.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(255, 255, 255);
    openNIDevice.drawDebug(); // debug draw does the equicalent of the commented methods below
    //openNIDevice.drawDepth(0, 0, 640, 480);
    //openNIDevice.drawImage(640, 0, 640, 480);
    //openNIDevice.drawSkeletons(640, 0, 640, 480);
    
    // do some drawing of user clouds and masks
    ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    int numUsers = openNIDevice.getNumTrackedUsers();
    for (int nID = 0; nID < numUsers; nID++){
        ofxOpenNIUser & user = openNIDevice.getTrackedUser(nID);
        user.drawMask();
        ofPushMatrix();
        ofTranslate(320, 240, -1000);
        user.drawPointCloud();
        ofPopMatrix();
    }
    ofDisableBlendMode();
    ofPopMatrix();
    
	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
	verdana.drawString(msg, 20, 506);
}

//--------------------------------------------------------------
void testApp::exit(){
    // this often does not work -> it's a known bug -> but calling it on a key press or 
    // anywhere that isnt std::aexit() works...press 'x' to shutdown cleanly...
    openNIDevice.stop();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'x':
            openNIDevice.stop();
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

