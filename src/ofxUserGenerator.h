#ifndef _H_OFXUSERGENERATOR
#define _H_OFXUSERGENERATOR

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"

#define MAX_NUMBER_USERS 8

class ofxTrackedUser;

class ofxUserGenerator {

public:
	
	ofxUserGenerator();
	~ofxUserGenerator(){};
	
	bool				setup(ofxOpenNIContext* pContext);
	
	void				draw();
	void				update();

	void				startTracking(XnUserID nID);
	void				startPoseDetection(XnUserID nID);
	void				stopPoseDetection(XnUserID nID);
	void				requestCalibration(XnUserID nID);
	bool				needsPoseForCalibration();
	
	void				setUseMaskPixels(bool b);
	void				setUseCloudPoints(bool b);
	
	void				setSmoothing(float smooth);
	float				getSmoothing();
	
	xn::UserGenerator&	getXnUserGenerator();
	
	void				setMaxNumberOfUsers(int nUsers);
	int					getNumberOfTrackedUsers();
	ofxTrackedUser*		getTrackedUser(int nUserNum);
	unsigned char *		getUserPixels(int userID = 0);
	ofPoint				getWorldCoordinateAt(int x, int y, int userID = 0);
	ofColor				getWorldColorAt(int x, int y, int userID = 0);
	
	int					getWidth();
	int					getHeight();
	
private:
	
	void				drawUser(int nUserNum);
	void				updateUserPixels();
	void				updateCloudPoints();
	
	ofxOpenNIContext*	context;
	xn::DepthGenerator	depth_generator;
	xn::ImageGenerator	image_generator;
	xn::UserGenerator	user_generator;
	
	// vars for user tracking
	XnBool							needs_pose;
	XnChar							calibration_pose[20];
	ofxTrackedUser *				tracked_users[MAX_NUMBER_USERS];
	XnUInt16						found_users;
	int								max_num_users;
	
	// vars for cloud point and masking
	XnUInt16			width, height;
	unsigned char *		maskPixels[MAX_NUMBER_USERS+1];//including 0 as all users
	ofPoint		*		cloudPoints[MAX_NUMBER_USERS+1];//including 0 as all users
	ofColor		*		cloudColors[MAX_NUMBER_USERS+1];//including 0 as all users
	bool				useMaskPixels, useCloudPoints;
	
	float				smoothing_factor;
	
};

#endif