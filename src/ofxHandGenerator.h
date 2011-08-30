#ifndef _H_OFXHANDGENERATOR
#define _H_OFXHANDGENERATOR

//#define USE_OPENNI_SMOOTHING

#include "ofxOpenNIContext.h"
#include "ofxGestureGenerator.h"

class ofxTrackedHand;
class ofxHandGenerator {

public:
	
	ofxHandGenerator();
	~ofxHandGenerator();
	
	bool				setup(ofxOpenNIContext* pContext, int number_of_hands = 1);
	
	void				dropHands();
	void				toggleTrackHands();
	void				startTrackHands();
	void				stopTrackHands();
	
	void				drawHands();
	void				drawHand(int thIndex);
	
	ofxTrackedHand*		getHand(int thIndex);
	int					getNumTrackedHands();
	
	void				setSmoothing(float smooth);						// refers to built in openni smoothing (float between 0.0 and 1.0)
	float				getSmoothing();
	
	void				setMaxNumHands(int number);
	int					getMaxNumHands();
	
	void				setMinDistBetweenHands(int distance);
	int					getMinDistBetweenHands();
	
	void				setMinTimeBetweenHands(int time);
	int					getMinTimeBetweenHands();
	
	void				setFilterFactors(float factor);					// sets user filtering factor on all hands
	void				setFilterFactor(float factor, int thIndex);		// sets user filtering factor on one hand
	
	bool				isFiltering;									// refers to our user filtering in the ofxTrackedHand class
	
	// CALLBACK PRIVATES
	void startHandTracking(const XnPoint3D* pPosition);
	void newHand(XnUserID nID, const XnPoint3D* pPosition);
	void updateHand(XnUserID nID, const XnPoint3D* pPosition);
	void destroyHand(XnUserID nID);
	
	std::vector<ofxTrackedHand*>	tracked_hands;		// TODO: make this an array!

	xn::HandsGenerator&				getXnHandsGenerator();
	
private:
	
	void addGestures();
	void removeGestures();
	
	ofxOpenNIContext*			context;
	
	xn::DepthGenerator			depth_generator;
	xn::HandsGenerator			hands_generator;
	
	ofxGestureGenerator			gesture_generator;
	void						gestureRecognized(gesture & last_gesture);
	
	int							min_time;
	
	int							found_hands;
	
	float						smoothing_factor;
	int							max_hands;
	int							min_distance;
	
	XnCallbackHandle			hand_cb_handle;
	
	bool						bIsTracking;
	
	ofxHandGenerator(ofxHandGenerator const& other);
	ofxHandGenerator & operator = (const ofxHandGenerator&);

};

#endif