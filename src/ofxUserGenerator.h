/*
 * ofxUserGenerator.h
 *
 * Copyright 2011 (c) Matthew Gingold http://gingold.com.au
 * Originally forked from a project by roxlu http://www.roxlu.com/ 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

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
	
	void				draw(const int width=640, const int height=480);
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
	
	void				drawUser(int nUserNum, const float wScale=1.0f, const float hScale=1.0f);
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
