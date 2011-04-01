#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	
	isLive		= true;
	isTracking	= false;
	isRecording = false;
	isCloud		= false;
	isCPBkgnd	= true;
	isMasking   = true;
	
	nearThreshold = 500;
	farThreshold  = 1000;
	
	setupRecording();
	
	ofBackground(0, 0, 0);
	
}

void testApp::setupRecording(string _filename) {
		
	recordContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	recordDepth.setup(&recordContext);
	recordImage.setup(&recordContext);
	recordUser.setup(&recordContext);
	recordUser.setUseMaskPixels(isMasking);
	recordUser.setUseCloudPoints(isCloud);
	recordContext.toggleRegisterViewport();
	recordContext.toggleMirror();
	
	oniRecorder.setup(&recordContext, ONI_STREAMING);	
	//oniRecorder.setup(&recordContext, ONI_CYCLIC, 60); 
	//read the warning in ofxOpenNIRecorder about memory usage with ONI_CYCLIC recording!!!
	
}

void testApp::setupPlayback(string _filename) {
	
	playContext.shutdown();
	playContext.setupUsingRecording(ofToDataPath(_filename));
	playDepth.setup(&playContext);
	playImage.setup(&playContext);
	playUser.setup(&playContext);
	playUser.setUseMaskPixels(isMasking);
	playUser.setUseCloudPoints(isCloud);
	playContext.toggleRegisterViewport();
	playContext.toggleMirror();
	
}

//--------------------------------------------------------------
void testApp::update(){

	if (isLive) {
		
		// update all nodes
		recordContext.update();
		recordDepth.update();
		recordImage.update();
		
		// demo getting depth pixels directly from depth gen
		depthRangeMask.setFromPixels(recordDepth.getDepthPixels(nearThreshold, farThreshold), 
									 recordDepth.getWidth(), recordDepth.getHeight(), OF_IMAGE_GRAYSCALE);
		
		// update tracking/recording nodes
		if (isTracking) recordUser.update();
		if (isRecording) oniRecorder.update();
		
		// demo getting pixels from user gen
		if (isTracking && isMasking) {
			allUserMasks.setFromPixels(recordUser.getUserPixels(), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user1Mask.setFromPixels(recordUser.getUserPixels(1), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user2Mask.setFromPixels(recordUser.getUserPixels(2), recordUser.getWidth(), recordUser.getHeight(), OF_IMAGE_GRAYSCALE);
		}
		
	} else {
		
		// update all nodes
		playContext.update();
		playDepth.update();
		playImage.update();
		
		// demo getting depth pixels directly from depth gen
		depthRangeMask.setFromPixels(playDepth.getDepthPixels(nearThreshold, farThreshold), 
									 playDepth.getWidth(), playDepth.getHeight(), OF_IMAGE_GRAYSCALE);
		
		// update tracking/recording nodes
		if (isTracking) playUser.update();
		
		// demo getting pixels from user gen
		if (isTracking && isMasking) {
			allUserMasks.setFromPixels(playUser.getUserPixels(), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user1Mask.setFromPixels(playUser.getUserPixels(1), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
			user2Mask.setFromPixels(playUser.getUserPixels(2), playUser.getWidth(), playUser.getHeight(), OF_IMAGE_GRAYSCALE);
		}
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
		
		depthRangeMask.draw(0, 480, 320, 240);	// can use this with openCV to make masks, find contours etc when not dealing with openNI 'User' like objects
		
		if (isTracking) {
			
			recordUser.draw();
			
			if (isMasking) drawMasks();
			if (isCloud) drawPointCloud(&recordUser, 1);	// 0 gives you all point clouds; use userID to see point clouds for specific users
			
		}
		
	} else {
		
		playDepth.draw(0,0,640,480);
		playImage.draw(640, 0, 640, 480);
		
		depthRangeMask.draw(0, 480, 320, 240);	// can use this with openCV to make masks, find contours etc when not dealing with openNI 'User' like objects

		if (isTracking) {
			
			playUser.draw();
			
			if (isMasking) drawMasks();
			if (isCloud) drawPointCloud(&playUser, 0);	// 0 gives you all point clouds; use userID to see point clouds for specific users

		}

	}
	
	glPopMatrix();
	
	ofSetColor(255, 255, 0);
	
	string msg1, msg2, msg3;	//drawBitmapString is limited to some numebr of characters -> is this a bug in 007 or always the case?
	
	msg1 += "Press 's' to start/stop recording\n";
	msg1 += "Press 'p' to toggle playback/live streams\n";
	msg1 += "Press 't' to toggle tracking\n";
	msg2 += "Press 'm' to toggle drawing Masks\n";
	msg2 += "Press 'c' to toggle draw Cloud points\n";
	msg2 += "Press 'b' to toggle Cloud User data (ie., background removal)\n\n";
	
	msg3 += (string)(isTracking ? "TRACKING USERS: " + (string)(isLive ? ofToString(recordUser.getNumberOfTrackedUsers()) : ofToString(playUser.getNumberOfTrackedUsers())) + "\n" : "NOT TRACKING USERS\n");
	msg3 += (string)(isLive ? "LIVE STREAM\n" : "PLAY STREAM\n");
	msg3 += (string)(!isRecording ? "READY\n" : "RECORDING\n");
	msg3 += "MASK DRAWING: " + (string)(!isMasking ? "HIDE\n" : (string)(isTracking ? "SHOW\n" : "YOU NEED TO TURN ON TRACKING!!\n"));
	msg3 += "CLOUD DRAWING: " + (string)(isCloud ? "ON\n" : "OFF\n");
	msg3 += "CLOUD USERDATA/BACKGROUND: " + (string)(isCPBkgnd ? "SHOW BACKGROUND\n" : (string)(isTracking ? "SHOW USER\n" : "YOU NEED TO TURN ON TRACKING!!\n"));
	msg3 += "nearThreshold: " + ofToString(nearThreshold) + " farThreshold: " + ofToString(farThreshold) + "\n";
	msg3 += "FPS: " + ofToString(ofGetFrameRate()) + "\n";
	
	ofDrawBitmapString(msg1, 20, 600);
	ofDrawBitmapString(msg2, 20, 650);
	ofDrawBitmapString(msg3, 20, 700);
	ofDrawBitmapString(oniRecorder.getCurrentFileName(), 20, 720);

	
}

void testApp:: drawMasks() {
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_DST_COLOR, GL_ZERO);
	allUserMasks.draw(640, 0);
	glDisable(GL_BLEND);
	user1Mask.draw(320, 480, 320, 240);
	user2Mask.draw(640, 480, 320, 240);
	glPopMatrix();
}

void testApp::drawPointCloud(ofxUserGenerator * user_generator, int userID) {
	
	glPushMatrix();
	
	int w = user_generator->getWidth();
	int h = user_generator->getHeight();
	
	glTranslatef(w, h/2, -500);
	ofRotateY(pointCloudRotationY);
	
	glBegin(GL_POINTS);
	
	int step = 1;

	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			ofPoint pos = user_generator->getWorldCoordinateAt(x, y, userID);
			if (pos.z == 0 && isCPBkgnd) continue;	// gets rid of background -> still a bit weird if userID > 0...
			ofColor color = user_generator->getWorldColorAt(x,y, userID);
			glColor4ub((unsigned char)color.r, (unsigned char)color.g, (unsigned char)color.b, (unsigned char)color.a);
			glVertex3f(pos.x, pos.y, pos.z);
		}
	}
	
	glEnd();
	
	glColor3f(1.0f, 1.0f, 1.0f);
	
	glPopMatrix();
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
				oniRecorder.startRecord(generateFileName());
				isRecording = true;
				break;
			}
			break;
		case 'p':
			if (oniRecorder.getCurrentFileName() != "" && !isRecording && isLive) {
				setupPlayback(oniRecorder.getCurrentFileName());
				isLive = false;
			} else {
				isLive = true;
			}
			break;
		case 't':
			isTracking = !isTracking;
			break;
		case 'm':
			isMasking = !isMasking;
			recordUser.setUseMaskPixels(isMasking);
			playUser.setUseMaskPixels(isMasking);
			break;
		case 'c':
			isCloud = !isCloud;
			recordUser.setUseCloudPoints(isCloud);
			playUser.setUseCloudPoints(isCloud);
			break;
		case 'b':
			isCPBkgnd = !isCPBkgnd;
			break;
		case '>':
		case '.':
			farThreshold += 50;
			if (farThreshold > recordDepth.getMaxDepth()) farThreshold = recordDepth.getMaxDepth();
			break;
			
		case '<':		
		case ',':		
			farThreshold -= 50;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold += 50;
			if (nearThreshold > recordDepth.getMaxDepth()) nearThreshold = recordDepth.getMaxDepth();
			break;
			
		case '-':		
			nearThreshold -= 50;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
		case 'r':		
			recordContext.toggleRegisterViewport();
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
	
	if (isCloud) pointCloudRotationY = x;

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

