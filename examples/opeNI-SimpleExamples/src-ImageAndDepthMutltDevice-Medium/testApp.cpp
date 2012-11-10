#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {

    ofSetLogLevel(OF_LOG_NOTICE);

    numDevices = openNIDevices[0].getNumDevices();

    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        //openNIDevices[deviceID].setLogLevel(OF_LOG_VERBOSE);
        openNIDevices[deviceID].setup();
        openNIDevices[deviceID].addDepthGenerator();
        openNIDevices[deviceID].addImageGenerator();
        openNIDevices[deviceID].setRegister(true); // this registers all the image pixels to the depth pixels
        openNIDevices[deviceID].setMirror(true); // flips the image and depth sensors
		openNIDevices[deviceID].start();
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
        openNIDevices[deviceID].drawDebug(); // draws all generators
        //openNIDevices[deviceID].drawDepth(0, 0);
        //openNIDevices[deviceID].drawImage(640, 0);
    }

    ofPopMatrix();

	ofSetColor(0, 255, 0);
	string msg = " MILLIS: " + ofToString(ofGetElapsedTimeMillis()) + " FPS: " + ofToString(ofGetFrameRate());
	verdana.drawString(msg, 20, numDevices * 480 - 20);
}

//--------------------------------------------------------------
void testApp::exit(){
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].stop();
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 't':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                openNIDevices[deviceID].toggleRegister();
            }
            break;
        case 'x':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                openNIDevices[deviceID].stop();
            }
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

