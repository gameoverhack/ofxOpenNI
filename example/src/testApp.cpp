#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	isLive		= true;
	isTracking	= false;
	isRecording = false;
	isCloud		= false;
	isCPBkgnd	= true;
	
	setupRecording();

	ofBackground(0, 0, 0);
	
}

void testApp::setupRecording(string _filename) {
	
	if (!recordContext.isInitialized()) {
		
		recordContext.setup();
		recordContext.setupUsingXMLFile();
		recordDepth.setup(&recordContext);
		recordImage.setup(&recordContext);
		
		recordUser.setup(&recordContext, &recordDepth, &recordImage);
		
		recordDepth.registerViewport(&recordImage);
		recordContext.toggleMirror();
		
		oniRecorder.setup(&recordContext, &recordDepth, &recordImage);	
		
	} else {
		
		currentFileName = _filename;
		cout << currentFileName << endl;
	}
		
}

void testApp::setupPlayback(string _filename) {
	
	playContext.clear();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);
	playUser.setup(&playContext, &playDepth, &playImage);
	
	
	playDepth.registerViewport(&playImage);
	playContext.toggleMirror();
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	if (isLive) {
		recordContext.update();
		if (isTracking) recordUser.update();
	} else {
		playContext.update();
		if (isTracking) playUser.update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(255, 255, 255);
	
	if (isLive) {
		
		recordDepth.draw(0,0,640,480);
		recordImage.draw(640, 0, 640, 480);
		if (isTracking) {
			recordUser.draw();
			ofxTrackedUser* tracked = recordUser.getTrackedUser(0);
			if(tracked != NULL) {
				tracked->debugDraw();
			}
		}
		
		if (isCloud) {
			recordUser.drawPointCloud(isCPBkgnd);
		}
		
	} else {
		
		playDepth.draw(0,0,640,480);
		playImage.draw(640, 0, 640, 480);
		if (isTracking) {
			playUser.draw();
			ofxTrackedUser* tracked = playUser.getTrackedUser(0);
			if(tracked != NULL) {
				tracked->debugDraw();
			}
		}
		
		if (isCloud) {
			playUser.drawPointCloud(isCPBkgnd);
		}
		
	}
	
	ofSetColor(255, 255, 0);
	
	string msg1, msg2;
	
	msg1 += "Press 's' to start/stop recording\n";
	msg1 += "Press 'p' to toggle playback of last file and live kinect streams\n";
	msg1 += "Press 't' to toggle tracking\n";
	msg1 += "Press 'c' to Cloud Point drawing and 'b' to toggle using User Data (ie., background removal)\n\n";
	msg2 += ofToString(isTracking ? "TRACKING USERS\n" : "NOT TRACKING USERS\n");
	msg2 += ofToString(isLive ? "LIVE STREAM\n" : "PLAY STREAM\n");
	msg2 += ofToString(!isRecording ? "READY\n" : "RECORDING\n");
	msg2 += "CLOUD DRAWING: " + ofToString(isCloud ? "ON" : "OFF");
	msg2 += " CLOUD USING USERDATA/BACKGROUND: " + ofToString(isCPBkgnd ? "SHOW BACKGROUND\n" : ofToString(isTracking ? "SHOW USER\n" : "YOU NEED TO TURN ON TRACKING!!\n"));
	msg2 += "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	
	ofDrawBitmapString(msg1, ofPoint(20, 500));
	ofDrawBitmapString(msg2, ofPoint(20, 600));
	ofDrawBitmapString(currentFileName, ofPoint(20, 700));

	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch (key) {
		case 's':
			if (isRecording) {
				oniRecorder.stopRecord();
				isRecording = false;
				break;
			} else {
				setupRecording(generateFileName());
				oniRecorder.startRecord(currentFileName);
				isRecording = true;
				break;
			}
			break;
		case 'p':
			if (currentFileName != "" && !isRecording && isLive) {
				setupPlayback(currentFileName);
				isLive = false;
			} else {
				isLive = true;
			}
			break;
		case 't':
			isTracking = !isTracking;
			break;
		case 'm':
			if (isLive) {
				recordContext.toggleMirror();
			} else {
				playContext.toggleMirror();
			}
			break;
		case 'c':
			isCloud = !isCloud;
			break;
		case 'b':
			isCPBkgnd = !isCPBkgnd;
			break;

		default:
			break;
	}	
}

string testApp::generateFileName() {
	
	string _root = "kinectRecord";
	
	string _timestamp = ofGetTimestampString("%Y%m%d%H%M%S");
	
	string _filename = (_root + _timestamp + ".oni");
	
	return _filename;
	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
	if (isLive && isCloud) {
		recordUser.setPointCloudRotation(x);
	} else if (!isLive && isCloud) {
		playUser.setPointCloudRotation(x);
	}
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

