#pragma once

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxGestureGenerator.h"


//
// If MAX_NUMBER_HANDS > 1, gesture triggers several Gesture_Recognized callbacks
// So, to detect more hands, we need to figure out how to identify if a gesture
// is from the same hand or from another hand.
// Strange thing is, if MAX_NUMBER_HANDS is 1, Gesture_Recognized fires only once
#define MAX_NUMBER_HANDS	1

//
// One tracked Hand
class ofxTrackedHand {
public:
	
	ofxTrackedHand(ofxDepthGenerator* pDepthGenerator);

	void update(const XnPoint3D* pPosition, bool force=false);

	void draw();
	
	ofxDepthGenerator* depth_generator;
	
	bool isBeingTracked;
	bool isFiltering;
	XnUserID nID;
	XnPoint3D rawPos;
	ofPoint projectPos;		// position on screen
	ofPoint progPos;		// position from 0.0 to 1.0
	
};


//
// Gesture Generator
class ofxGestureGenerator {
public:
	ofxGestureGenerator();
	~ofxGestureGenerator();
	
	bool setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator);
	
	void addGestures();
	
	void removeGestures();

	void startHandTracking(const XnPoint3D* pPosition);
	
	void newHand(XnUserID nID, const XnPoint3D* pPosition);

	void updateHand(XnUserID nID, const XnPoint3D* pPosition);
	
	void destroyHand(XnUserID nID);
	
	void drawHands();
	
	void drawHand(int nHandNum);
	
	int getHandsCount() { return found_hands; };

	ofxTrackedHand* getHand();
	
	xn::GestureGenerator& getXnGestureGenerator();
	
private:
	
	ofxOpenNIContext* context;
	ofxDepthGenerator* depth_generator;

	xn::GestureGenerator gestureGenerator;
	xn::HandsGenerator handsGenerator;

	std::vector<ofxTrackedHand*> tracked_hands;
	XnUInt16 num_hands;
	int found_hands;

};

