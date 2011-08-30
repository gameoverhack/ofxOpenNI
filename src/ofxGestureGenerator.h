#ifndef _H_OFXGESTUREGENERATOR
#define _H_OFXGESTUREGENERATOR

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofEvents.h"

enum {
	GESTURE_RECOGNIZED,
	GESTURE_PROGRESS
};

typedef struct {
	string	gesture_name;
	int		gesture_type;
	float	gesture_progress;
	ofPoint	gesture_position;
	int		gesture_timestamp;
} gesture;

class ofxGestureGenerator {
	
public:
	
	ofxGestureGenerator();
	~ofxGestureGenerator();
	
	bool					setup(ofxOpenNIContext* pContext);

	bool					addGesture(string gesture_name);
	bool					removeGesture(string gesture_name);
	
	void					setMinTimeBetweenGestures(int time);
	int						getMinTimeBetweenGestures();
	
	xn::GestureGenerator&	getXnGestureGenerator();
	
	ofEvent<gesture>				gestureRecognized;
	ofEvent<gesture>				gestureProgress;
	
	// PRIVATE CALLBACK
	
	gesture		*				getLastGesture();
	
	void						setUseProgress(bool b);
	bool						getUseProgress();
	
	void						setVerbose(bool b);	
	
private:
	
	ofxOpenNIContext*		context;
	xn::GestureGenerator	gesture_generator;
	xn::DepthGenerator		depth_generator;
	
	gesture					last_gesture;
	bool					b_use_progress;
	int						min_time;
	
	bool					b_verbose;

	ofxGestureGenerator(ofxGestureGenerator const& other);
	ofxGestureGenerator & operator = (const ofxGestureGenerator&);
	
};


#endif