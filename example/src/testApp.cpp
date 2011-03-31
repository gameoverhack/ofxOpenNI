#include "testApp.h"

// ROGER:: REMOVE THIS!!!
bool status = true;

//--------------------------------------------------------------
void testApp::setup() {
	
	isLive				= true;
	isTracking			= true;
	isTrackingHands		= true;
	isRecording			= false;
	isCloud				= false;
	isCPBkgnd			= true;
	isMasking			= true;
	
	setupRecording();

	ofBackground(0, 0, 0);
	
}

void testApp::setupRecording(string _filename) {
	
	if (!recordContext.isInitialized()) {
		
		recordContext.setup();
		recordContext.setupUsingXMLFile();
		//recordContext.enableLogging(XN_LOG_WARNING);

		recordDepth.setup(&recordContext);
		recordImage.setup(&recordContext);
		recordUser.setup(&recordContext, &recordDepth, &recordImage);
		recordGesture.setup(&recordContext, &recordDepth);
		
		recordDepth.toggleRegisterViewport(&recordImage);
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
	playGesture.setup(&playContext, &playDepth);

	
	playDepth.toggleRegisterViewport(&playImage);
	playContext.toggleMirror();
	
}

//--------------------------------------------------------------
void testApp::update(){
	
	if (isLive) {
		recordContext.update();
		recordDepth.update();
		if (isTracking) recordUser.update();
	} else {
		playContext.update();
		playDepth.update();
		if (isTracking) playUser.update();
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofSetColor(255, 255, 255);
	
	glPushMatrix();
	glScalef(0.75, 0.75, 0.75);
	
	if (isLive) {
		
		recordDepth.draw(0,0,640,480);
		recordImage.draw(640, 0, 640, 480);
		if (isTracking) {
			
			recordUser.draw();
			
			if(isMasking) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				recordUser.drawUserMasks(640, 0);
				glDisable(GL_BLEND);
			}
		}
		if (isTrackingHands)
		{
			recordGesture.drawHands();
		}
		
		if (isCloud) {
			recordUser.drawPointCloud(isCPBkgnd);
		}
		
	} else {
		
		playDepth.draw(0,0,640,480);
		playImage.draw(640, 0, 640, 480);
		if (isTracking) {
			
			playUser.draw();
			
			if(isMasking) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_DST_COLOR, GL_ZERO);
				playUser.drawUserMasks(640, 0);
				glDisable(GL_BLEND);
			}
		}
		if (isTrackingHands)
		{
			playGesture.drawHands();
		}
		
		if (isCloud) {
			playUser.drawPointCloud(isCPBkgnd);
		}
		
	}
	
	glPopMatrix();
	
	ofSetColor(255, 255, 0);
	
	string statusPlay		= (string)(isLive ? "LIVE STREAM\n" : "PLAY STREAM\n");
	string statusRec		= (string)(!isRecording ? "READY\n" : "RECORDING\n");
	string statusSkeleton	= (string)(isTracking ? "TRACKING USERS\n" : "NOT TRACKING\n");
	string statusHands		= (string)(isTrackingHands ? "TRACKING HANDS\n" : "NOT TRACKING\n");
	string statusMask		= (string)(!isMasking ? "HIDE\n" : (isTracking ? "SHOW\n" : "YOU NEED TO TURN ON TRACKING!!\n"));
	string statusCloud		= (string)(isCloud ? "ON\n" : "OFF\n");
	string statusCloudData	= (string)(isCPBkgnd ? "SHOW BACKGROUND\n" : (isTracking ? "SHOW USER\n" : "YOU NEED TO TURN ON TRACKING!!\n"));

	stringstream msg;
	msg
	<< "Press 's' to start/stop recording          : " << statusRec << endl
	<< "Press 't' to toggle skeleton tracking      : " << statusSkeleton << endl
	<< "Press 'h' to toggle hand tracking          : " << statusHands << endl
	<< "Press 'm' to toggle drawing Masks          : " << statusMask << endl
	<< "Press 'c' to toggle draw Cloud points      : " << statusCloud << endl
	<< "Press 'b' to toggle Cloud User data        : " << statusCloudData << endl
	<< endl
	<< "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	
	ofDrawBitmapString(msg.str(), 20, 500);
	ofDrawBitmapString(currentFileName, 20, 700);

	
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
		case 'h':
			isTrackingHands = !isTrackingHands;
			break;
		case 't':
			isTracking = !isTracking;
			break;
		case 'm':
			isMasking = !isMasking;
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
	
	string _timestamp = ofToString(ofGetDay()) + 
						ofToString(ofGetMonth()) +
						ofToString(ofGetYear()) +
						ofToString(ofGetHours()) +
						ofToString(ofGetMinutes()) +
						ofToString(ofGetSeconds());
	
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

