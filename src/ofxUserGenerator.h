#ifndef USERGENERATORH
#define USERGENERATORH

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include <vector>

class ofxTrackedUser;

class ofxUserGenerator {
public:
	ofxUserGenerator();
	bool setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator);
	void draw();
	void update();
	void requestCalibration(XnUserID nID);
	bool needsPoseForCalibration();
	void startPoseDetection(XnUserID nID);
	void stopPoseDetection(XnUserID nID);
	void startTracking(XnUserID nID);
	xn::UserGenerator* getXnUserGenerator();
	ofxTrackedUser* getTrackedUser(int nUserNum);

private:	
	void drawUsers();
	void drawUser(int nUserNum);
	
	XnBool needs_pose;
	xn::UserGenerator user_generator;
	ofxOpenNIContext* context;
	ofxDepthGenerator* depth_generator;
	XnChar calibration_pose[20];
	std::vector<ofxTrackedUser*> tracked_users;
	XnUInt16 num_users;
	XnUInt16 found_users;

};
#endif

