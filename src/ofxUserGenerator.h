#pragma once

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"
#include "ofxCvGrayscaleImage.h"

#include <vector>

#define MAX_NUMBER_USERS 15

class ofxTrackedUser;

class ofxUserGenerator {
public:
	ofxUserGenerator();
	
	bool setup(ofxOpenNIContext* pContext);
	
	void draw();
	void drawUserMasks(int x, int y);
	void drawPointCloud(bool showBackground, int cloudPointSize = 1);
	
	void update();
	void updateUserMask(int userID);
	
	void setPointCloudRotation(int _x);
	
	void startTracking(XnUserID nID);
	void startPoseDetection(XnUserID nID);
	void stopPoseDetection(XnUserID nID);
	
	void requestCalibration(XnUserID nID);
	bool needsPoseForCalibration();
	
	xn::UserGenerator& getXnUserGenerator();
	
	int				getNumberOfTrackedUsers();
	ofxTrackedUser* getTrackedUser(int nUserNum);

private:
	
	void drawUser(int nUserNum);
	
	ofxOpenNIContext*	context;
	xn::DepthGenerator	depth_generator;
	xn::ImageGenerator	image_generator;
	xn::UserGenerator	user_generator;
	
	// vars for user tracking
	XnBool							needs_pose;
	XnChar							calibration_pose[20];
	ofxTrackedUser *				tracked_users[MAX_NUMBER_USERS];
	XnUInt16						num_users;
	XnUInt16						found_users;

	// vars for cloud point and masking
	XnUInt16			width, height;
	unsigned char *		maskPixels;
	ofxCvGrayscaleImage maskImage[MAX_NUMBER_USERS];
	int					cloudPointRotationY;
	
};


