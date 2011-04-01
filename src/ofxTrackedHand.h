#ifndef _H_OFXTRACKEDHAND
#define _H_OFXTRACKEDHAND

#include "ofMain.h"
#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include "ofxOpenNIContext.h"

// This is ready for more hands, but.....
// If MAX_NUMBER_HANDS > 1, gesture triggers several Gesture_Recognized callbacks
// So, to detect more hands, we need to figure out how to identify if a gesture
// is from the same hand or from another hand.
// Strange thing is, if MAX_NUMBER_HANDS is 1, Gesture_Recognized fires only once

class ofxOpenNIContext;
class ofxTrackedHand {
	
public:
	
	void update(const XnPoint3D* pPosition, bool filter=false, bool force=false);
	
	void draw();
	
	bool				isBeingTracked;
	XnUserID			nID;
	XnPoint3D			rawPos;
	ofPoint				projectPos;		// position on screen
	ofPoint				progPos;		// position from 0.0 to 1.0
	
private:
	
	ofxTrackedHand(ofxOpenNIContext* pContext);
	
	xn::DepthGenerator	depth_generator;
	
	float				xres, yres, zres;
	
	friend class ofxHandGenerator;
};		

#endif