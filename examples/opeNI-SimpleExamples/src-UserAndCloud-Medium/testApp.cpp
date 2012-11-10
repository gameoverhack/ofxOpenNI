#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    
    ofSetLogLevel(OF_LOG_NOTICE);
    
    numDevices = openNIDevices[0].getNumDevices();
    
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        //openNIDevices[deviceID].setLogLevel(OF_LOG_VERBOSE); // ofxOpenNI defaults to ofLogLevel, but you can force to any level
        openNIDevices[deviceID].setup();
        openNIDevices[deviceID].addDepthGenerator();
        openNIDevices[deviceID].addImageGenerator();
        openNIDevices[deviceID].addUserGenerator();
        openNIDevices[deviceID].setRegister(true);
        openNIDevices[deviceID].setMirror(true);
		openNIDevices[deviceID].start();
    }
    
    // NB: Only one device can have a user generator at a time - this is a known bug in NITE due to a singleton issue
    // so it's safe to assume that the fist device to ask (ie., deviceID == 0) will have the user generator...
    
    openNIDevices[0].setMaxNumUsers(1); // defualt is 4
    ofAddListener(openNIDevices[0].userEvent, this, &testApp::userEvent);
    
    ofxOpenNIUser user;
    user.setUseMaskTexture(true);
    user.setUsePointCloud(true);
    user.setPointCloudDrawSize(2); // this is the size of the glPoint that will be drawn for the point cloud
    user.setPointCloudResolution(2); // this is the step size between points for the cloud -> eg., this sets it to every second point
    openNIDevices[0].setBaseUserClass(user); // this becomes the base class on which tracked users are created
                                             // allows you to set all tracked user properties to the same type easily
                                             // and allows you to create your own user class that inherits from ofxOpenNIUser
    
    // if you want to get fine grain control over each possible tracked user for some reason you can iterate
    // through users like I'm doing below. Please not the use of nID = 1 AND nID <= openNIDevices[0].getMaxNumUsers()
    // as what you're doing here is retrieving a user that is being stored in a std::map using it's XnUserID as the key
    // that means it's not a 0 based vector, but instead starts at 1 and goes upto, and includes maxNumUsers...
//    for (XnUserID nID = 1; nID <= openNIDevices[0].getMaxNumUsers(); nID++){
//        ofxOpenNIUser & user = openNIDevices[0].getUser(nID);
//        user.setUseMaskTexture(true);
//        user.setUsePointCloud(true);
//        //user.setUseAutoCalibration(false); // defualts to true; set to false to force pose detection
//        //user.setLimbDetectionConfidence(0.9f); // defaults 0.3f
//        user.setPointCloudDrawSize(2);
//        user.setPointCloudResolution(1);
//    }
    verdana.loadFont(ofToDataPath("verdana.ttf"), 24);
}

//--------------------------------------------------------------
void testApp::update(){
    ofBackground(0, 0, 0);
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
        openNIDevices[deviceID].drawDebug(); // debug draw does the equicalent of the commented methods below
//        openNIDevices[deviceID].drawDepth(0, 0, 640, 480);
//        openNIDevices[deviceID].drawImage(640, 0, 640, 480);
//        openNIDevices[deviceID].drawSkeletons(640, 0, 640, 480);
        
    }

    // do some drawing of user clouds and masks
    ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
    int numUsers = openNIDevices[0].getNumTrackedUsers();
    for (int nID = 0; nID < numUsers; nID++){
        ofxOpenNIUser & user = openNIDevices[0].getTrackedUser(nID);
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
	verdana.drawString(msg, 20, numDevices * 480 + 26);
}

//--------------------------------------------------------------
void testApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void testApp::exit(){
    // this often does not work -> it's a known bug -> but calling it on a key press or anywhere that isnt std::aexit() works
    // press 'x' to shutdown cleanly...
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].stop();
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    int cloudRes = -1;
    switch (key) {
        case '1':
            cloudRes = 1;
            break;
        case '2':
            cloudRes = 2;
            break;
        case '3':
            cloudRes = 3;
            break;
        case '4':
            cloudRes = 4;
            break;
        case '5':
            cloudRes = 5;
            break;
        case 'x':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                openNIDevices[deviceID].stop();
            }
            break;
        case 'i':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                if (openNIDevices[deviceID].isImageOn()){
                    openNIDevices[deviceID].removeImageGenerator();
                    openNIDevices[deviceID].addInfraGenerator();
                    continue;
                }
                if (openNIDevices[deviceID].isInfraOn()){
                    openNIDevices[deviceID].removeInfraGenerator();
                    openNIDevices[deviceID].addImageGenerator();
                    continue;
                }
            }
            break;
        case 'b':
            for (int deviceID = 0; deviceID < numDevices; deviceID++){
                openNIDevices[deviceID].setUseBackBuffer(!openNIDevices[deviceID].getUseBackBuffer());
            }
            break;
        default:
            break;
    }
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
		openNIDevices[deviceID].setPointCloudResolutionAllUsers(cloudRes);
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

