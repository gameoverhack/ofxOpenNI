#ifndef _TEST_APP
#define _TEST_APP


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
		
		string				currentFileName;
	
		ofxOpenNIContext	recordContext, playContext;
		ofxDepthGenerator	recordDepth, playDepth;
		ofxImageGenerator	recordImage, playImage;
		ofxUserGenerator	recordUser, playUser;
		ofxOpenNIRecorder	oniRecorder;

		
};

#endif
