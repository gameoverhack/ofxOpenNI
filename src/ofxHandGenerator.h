#ifndef _H_OFXHANDTRACKER
#define _H_OFXHANDTRACKER

#define MAX_NUMBER_HANDS 2
#define MIN_DIST_BETWEEN_HANDS 100

#include "ofxOpenNIContext.h"

class ofxTrackedHand;
class ofxHandGenerator {

public:
	
	ofxHandGenerator();
	~ofxHandGenerator();
	
	bool				setup(ofxOpenNIContext* pContext);
	
	void				dropHands();
	void				drawHands();
	void				drawHand(int nID);
	
	ofxTrackedHand*		getHand(int nID);
	int					getNumberofTrackedHands();
	
	bool				isFiltering;
	
	// CALLBACK PRIVATES
	void startHandTracking(const XnPoint3D* pPosition);
	void newHand(XnUserID nID, const XnPoint3D* pPosition);
	void updateHand(XnUserID nID, const XnPoint3D* pPosition);
	void destroyHand(XnUserID nID);
	
	std::vector<ofxTrackedHand*>	tracked_hands;

	xn::HandsGenerator&				getXnHandsGenerator();
	
private:
	
	void addGestures();
	void removeGestures();
	
	ofxOpenNIContext*			context;
	
	xn::DepthGenerator			depth_generator;
	xn::GestureGenerator		gesture_generator;
	xn::HandsGenerator			hands_generator;

	
	int							found_hands;
};

#endif