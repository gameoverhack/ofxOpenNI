#ifndef _TEST_APP
#define _TEST_APP

//#define USE_IR // Uncomment this to use infra red instead of RGB cam...

#include "ofMain.h"
#include "ofxOpenNI.h"


class testApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	void	setupRecording(string _filename = "");
	void	setupPlayback(string _filename);
	string	generateFileName();
	
	bool				isLive, isTracking, isRecording, isCloud, isCPBkgnd, isMasking;
	bool				isTrackingHands, isFiltering;
	
	ofxOpenNIContext	recordContext, playContext;
	ofxDepthGenerator	recordDepth, playDepth;

#ifdef USE_IR
	ofxIRGenerator		recordImage, playImage;
#else
	ofxImageGenerator	recordImage, playImage; 
#endif
	
	ofxHandGenerator	recordHandTracker, playHandTracker;
	
	ofxUserGenerator	recordUser, playUser;
	ofxOpenNIRecorder	oniRecorder;
#ifdef TARGET_OSX // only working on Mac at the moment
	ofxHardwareDriver	hardware;
#endif
	void				drawMasks();
	void				drawPointCloud(ofxUserGenerator * user_generator, int userID);
	
	int					nearThreshold, farThreshold;
	int					pointCloudRotationY;
	
	ofImage				allUserMasks, user1Mask, user2Mask, depthRangeMask;
	
	float				filterFactor;

};

#endif
