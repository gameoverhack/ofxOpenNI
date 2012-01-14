#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
	openNIDevice.setLogLevel(OF_LOG_VERBOSE);
    openNIDevice.setup();
    
    openNIDevice.addDepthGenerator();
    openNIDevice.addImageGenerator();
    openNIDevice.addUserGenerator();
    
    openNIDevice.setUsePointClouds(true);
}

//--------------------------------------------------------------
void testApp::update(){
    openNIDevice.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    
	ofSetColor(255, 255, 255);
    
    openNIDevice.drawDebug(); // draws all generators
//    openNIDevice.drawDepth(0, 0);
//    openNIDevice.drawImage(640, 0);
//    openNIDevice.drawUsers(0, 0);
    
    // draw point clouds
    int numUsers = openNIDevice.getNumUsers();
    for (int nID = 0; nID < numUsers; nID++){
        ofxOpenNIUser & user = openNIDevice.getUser(nID);
        glPointSize(2);
        ofPushMatrix();
        ofTranslate(0, 0, -1000); // center the points a bit
        glEnable(GL_DEPTH_TEST);
        user.pointCloud.drawVertices();
        glDisable(GL_DEPTH_TEST);
        ofPopMatrix();
        
    }
    
	ofSetColor(0, 255, 0);
	string msg = "FPS: " + ofToString(ofGetFrameRate());
	ofDrawBitmapString(msg, 20, ofGetHeight() - 20);
    
}

//--------------------------------------------------------------
void testApp::exit(){
    openNIDevice.stop(); // not clean yet 'cos dtor does not always fire correctly
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

