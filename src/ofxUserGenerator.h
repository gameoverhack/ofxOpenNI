#pragma once

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"

#include <vector>

class ofxTrackedUser;

class ofxUserGenerator {
public:
	ofxUserGenerator();
	
	bool setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator, ofxImageGenerator* image_generator);
	
	void draw();
	
	void drawPointCloud(bool showBackground, int cloudPointSize = 1);
	
	void setPointCloudRotation(int _x);
	
	void update();
	
	void requestCalibration(XnUserID nID);
	
	bool needsPoseForCalibration();
	
	void startPoseDetection(XnUserID nID);
	
	void stopPoseDetection(XnUserID nID);
	
	void startTracking(XnUserID nID);
	
	xn::UserGenerator& getXnUserGenerator();
	
	ofxTrackedUser* getTrackedUser(int nUserNum);
	
	std::vector<ofxTrackedUser*> getTrackedUsers();

private:	
	void drawUsers();
	void drawUser(int nUserNum);
	
	XnBool needs_pose;
	xn::UserGenerator user_generator;
	ofxOpenNIContext* context;
	ofxDepthGenerator* depth_generator;
	ofxImageGenerator* image_generator;
	XnChar calibration_pose[20];
	std::vector<ofxTrackedUser*> tracked_users;
	XnUInt16 num_users;
	XnUInt16 found_users;
	
	bool is_initialized;
	bool found_user;
	
	int cloudPointRotationY;
	
};


