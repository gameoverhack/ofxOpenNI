/*
 * ofxUserGenerator.cpp
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

#include "ofxUserGenerator.h"
#include "ofxOpenNIMacros.h"
#include "ofxTrackedUser.h"

// CALLBACKS
// =============================================================================
// Callback: New user was detected
void XN_CALLBACK_TYPE User_NewUser(
	xn::UserGenerator& rGenerator
	,XnUserID nID
	,void* pCookie
)
{
	printf("New User %d\n", nID);
	ofxUserGenerator* user = static_cast<ofxUserGenerator*>(pCookie);
	if(user->needsPoseForCalibration()) {
		user->startPoseDetection(nID);
	}
	else {
		user->requestCalibration(nID);	
	}
}

// Callback: An existing user was lost
void XN_CALLBACK_TYPE User_LostUser(
	xn::UserGenerator& rGenerator
	,XnUserID nID
	,void* pCookie
)
{
	printf("Lost user %d\n", nID);
	rGenerator.GetSkeletonCap().Reset(nID);
	
}

// Callback: Detected a pose
void XN_CALLBACK_TYPE UserPose_PoseDetected(
	xn::PoseDetectionCapability& rCapability
	,const XnChar* strPose
	,XnUserID nID
	,void* pCookie
)
{
	ofxUserGenerator* user = static_cast<ofxUserGenerator*>(pCookie);
	printf("Pose %s detected for user %d\n", strPose, nID);
	user->stopPoseDetection(nID);
	user->requestCalibration(nID);
}



void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(
	 xn::SkeletonCapability& capability
	,XnUserID nID
	,void* pCookie
)
{
	printf("Calibration started for user %d\n", nID);
}


void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(
	 xn::SkeletonCapability& rCapability
	,XnUserID nID
	,XnBool bSuccess
	,void* pCookie
)
{
	ofxUserGenerator* user = static_cast<ofxUserGenerator*>(pCookie);
	if(bSuccess) {
		printf("+++++++++++++++++++++++ Succesfully tracked user: %d\n", nID);
		user->startTracking(nID);
	}
	else {
		if(user->needsPoseForCalibration()) {
			user->startPoseDetection(nID);
		}
		else {
			user->requestCalibration(nID);	
		}
	}
}

// OFXUSERGENERATOR
// =============================================================================
ofxUserGenerator::ofxUserGenerator() {	
	needs_pose = false;
	max_num_users = MAX_NUMBER_USERS;
}


//----------------------------------------
void ofxUserGenerator::startPoseDetection(XnUserID nID) {
	printf("Start pose detection: %d +++++++++++++++++++++++++++++\n", nID);
	user_generator.GetPoseDetectionCap().StartPoseDetection(calibration_pose, nID);
}


//----------------------------------------
void ofxUserGenerator::stopPoseDetection(XnUserID nID) {
	user_generator.GetPoseDetectionCap().StopPoseDetection(nID);
}


//----------------------------------------
void ofxUserGenerator::requestCalibration(XnUserID nID) {
	user_generator.GetSkeletonCap().RequestCalibration(nID, TRUE);
}


// Setup the user generator.
//----------------------------------------
bool ofxUserGenerator::setup( ofxOpenNIContext* pContext) {
	
	bool ok = false;
	
	// store context and generator references
	context	= pContext;
	ok = context->getDepthGenerator(&depth_generator);
    if (!ok) return false;
    
	context->getImageGenerator(&image_generator);
	
	XnStatus result = XN_STATUS_OK;
	
	// get map_mode so we can setup width and height vars from depth gen size
	XnMapOutputMode map_mode; 
	depth_generator.GetMapOutputMode(map_mode);
	
	width = map_mode.nXRes;
	height = map_mode.nYRes;
	
	// set update mask pixels default to false
	useMaskPixels = false;
	
	// setup mask pixels array TODO: clean this up on closing or dtor
    //including 0 as all users
	for (int user = 0; user <= MAX_NUMBER_USERS; user++) {
		maskPixels[user] = new unsigned char[width * height];
	}
	
	// set update cloud points default to false
	useCloudPoints = false;
	
	// setup cloud points array TODO: clean this up on closing or dtor
    //including 0 as all users
	for (int user = 0; user <= MAX_NUMBER_USERS; user++) {
		cloudPoints[user] = new ofPoint[width * height];
		cloudColors[user] = new ofColor[width * height];
	}
	
	// check if the USER generator exists.
	if(!context->getUserGenerator(&user_generator)) {
		
		// if one doesn't exist then create user generator.
		result = user_generator.Create(context->getXnContext());
		CHECK_RC(result, "Create user generator");
		
		if (result != XN_STATUS_OK) return false;
	}	
	
	// register user callbacks
	XnCallbackHandle user_cb_handle;
	user_generator.RegisterUserCallbacks(
		 User_NewUser
		,User_LostUser
		,this
		,user_cb_handle
	);
	
	XnCallbackHandle calibration_cb_handle;
	user_generator.GetSkeletonCap().RegisterCalibrationCallbacks(
		 UserCalibration_CalibrationStart
		,UserCalibration_CalibrationEnd
		,this
		,calibration_cb_handle
	);
	
	// check if we need to pose for calibration
	if(user_generator.GetSkeletonCap().NeedPoseForCalibration()) {
		
		needs_pose = true;
		
		if(!user_generator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
			printf("Pose required, but not supported!\n");
			return false;
		}
		
		XnCallbackHandle user_pose_cb_handle;
		
		user_generator.GetPoseDetectionCap().RegisterToPoseCallbacks(
			 UserPose_PoseDetected
			,NULL
			,this
			,user_pose_cb_handle
		);
		
		user_generator.GetSkeletonCap().GetCalibrationPose(calibration_pose);
		
	}
	
	user_generator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
	
	// needs this to allow skeleton tracking when using pre-recorded .oni or nodes init'd by code (as opposed to xml)
	// as otherwise the image/depth nodes play but are not generating callbacks
	//if (context->isUsingRecording()) {
	result = context->getXnContext().StartGeneratingAll();
	CHECK_RC(result, "StartGenerating");

	// pre-generate the tracked users.
	for(int i = 0; i < MAX_NUMBER_USERS; ++i) {
		printf("Creting user: %i\n", i+1);
		ofxTrackedUser* tracked_user = new ofxTrackedUser(context);
		tracked_users[i] = tracked_user;
	}

	return true;
}


// Draw a specific user (start counting at 0)
//----------------------------------------
void ofxUserGenerator::drawUser(int nUserNum, const float wScale, const float hScale) {
	if(nUserNum - 1 > max_num_users)
		return;
	tracked_users[nUserNum]->updateBonePositions();
	tracked_users[nUserNum]->debugDraw(wScale, hScale);
}

// Draw all the found users.
//----------------------------------------
void ofxUserGenerator::draw(const int width, const int height) {
	
	// show green/red circle if any one is found
	if (found_users > 0) {
    const float wScale = width/640.0f;
    const float hScale = height/480.0f;
		
		// draw all the users
		for(int i = 0;  i < found_users; ++i) {
			drawUser(i, wScale, hScale);
		}
		
		glColor3f(0, 1.0f, 0);
		
	} else glColor3f(1.0f, 0, 0);
	
	ofCircle(10, 10, 10);
	
	// reset to white for simplicity
	glColor3f(1.0f, 1.0f, 1.0f);
	
}

// Get a tracked user.
//----------------------------------------
ofxTrackedUser* ofxUserGenerator::getTrackedUser(int nUserNum) {
	
	if(nUserNum - 1 > found_users)
		return NULL;
	return tracked_users[nUserNum - 1];
	
}

void ofxUserGenerator::setMaxNumberOfUsers(int nUsers) {
	// TODO: make this truly dynamic by replacing the define and writing dynamic allocation/deletion functions for the arrays! Lazy method below ;-)
	if (nUsers <= MAX_NUMBER_USERS) {
		max_num_users = nUsers;
	} else printf("Attempting to set number of tracked users higher than MAX_NUMBER_USERS - change the define in ofxUserGenerator.h first!");
}

// Get number of tracked users
int ofxUserGenerator::getNumberOfTrackedUsers() {
	return found_users;
}

// Update the tracked users, should be called each frame
//----------------------------------------
void ofxUserGenerator::update() {
	
	found_users = max_num_users;
	
	XnUserID* users = new XnUserID[max_num_users];
	user_generator.GetUsers(users, found_users);
	
	for(int i = 0; i < found_users; ++i) {
		if(user_generator.GetSkeletonCap().IsTracking(users[i])) {	
			tracked_users[i]->id = users[i];
			user_generator.GetCoM(users[i], tracked_users[i]->center);
            tracked_users[i]->skeletonTracking	  = user_generator.GetSkeletonCap().IsTracking(users[i]);
            tracked_users[i]->skeletonCalibrating = user_generator.GetSkeletonCap().IsCalibrating(users[i]);
            tracked_users[i]->skeletonCalibrated  = user_generator.GetSkeletonCap().IsCalibrated(users[i]);
            if(tracked_users[i]->skeletonTracking) tracked_users[i]->updateBonePositions();
		}
	}
	
	delete [] users;
	
	if (useMaskPixels) updateUserPixels();
	if (useCloudPoints) updateCloudPoints();
}

void ofxUserGenerator::setUseMaskPixels(bool b) {
	useMaskPixels = b;
}

void ofxUserGenerator::setUseCloudPoints(bool b) {
	useCloudPoints = b;
}

// return user pixels -> use 0 (default) to get all user masks
// or specify a number if you want seperate masks
//----------------------------------------
unsigned char * ofxUserGenerator::getUserPixels(int userID) {
	
	if (!useMaskPixels && userID == 0) {			// for people who just want all the user masks at once and don't want to waste the extra cycles looking through all users!!!
		
		xn::SceneMetaData smd;
		unsigned short *userPix;
		
		if (user_generator.GetUserPixels(0, smd) == XN_STATUS_OK) { 
			userPix = (unsigned short*)smd.Data();					
		}
		
		for (int i =0 ; i < width * height; i++) {
			if (userPix[i] == 0) {
				maskPixels[0][i] =  0;
			} else maskPixels[0][i] = 255;
			
			
		}
		
	}
	
	return maskPixels[userID];
}

// return user pixels -> use 0 (default) to get all user masks
// or specify a number if you want seperate masks
//----------------------------------------
void ofxUserGenerator::updateUserPixels() {
	
	xn::SceneMetaData smd;
	unsigned short *userPix;
	
	if (user_generator.GetUserPixels(0, smd) == XN_STATUS_OK) { //	GetUserPixels is supposed to take a user ID number,
		userPix = (unsigned short*)smd.Data();					//  but you get the same data no matter what you pass.
	}															//	userPix actually contains an array where each value
																//  corresponds to the user being tracked. 
																//  Ie.,	if userPix[i] == 0 then it's not being tracked -> it's the background!
																//			if userPix[i] > 0 then the pixel belongs to the user who's value IS userPix[i]
																//  // (many thanks to ascorbin who's code made this apparent to me)
	for (int i =0 ; i < width * height; i++) {
		
		// lets cycle through the users and allocate pixels into seperate masks for each user, including 0 as all users
		for (int user = 0; user <= max_num_users; user++) {
			if (userPix[i] == user) {
				maskPixels[user][i] = (user == 0 ? 0 : 255);
			} else maskPixels[user][i] = (user == 0 ? 255 : 0);
		}

	}
}

//----------------------------------------
void ofxUserGenerator::updateCloudPoints() {
	
	xn::DepthMetaData dm;
	xn::ImageMetaData im;
	
	const XnRGB24Pixel*		pColor;
	const XnDepthPixel*		pDepth;
	
	depth_generator.GetMetaData(dm);
	pDepth = dm.Data();
	
	bool hasImageGenerator = image_generator.IsValid();
	
	if (hasImageGenerator) {
		image_generator.GetMetaData(im);
		pColor = im.RGB24Data();
	}

	xn::SceneMetaData smd;
	unsigned short *userPix;
	
	if (user_generator.GetUserPixels(0, smd) == XN_STATUS_OK) {
		userPix = (unsigned short*)smd.Data();					
	}
	
	int step = 1;
	int nIndex = 0;
	
	for (int nY = 0; nY < height; nY += step) {
		
		for (int nX = 0; nX < width; nX += step, nIndex += step) {
		
			for (int user = 0; user <= max_num_users; user++) {

				if (userPix[nIndex] == user || user == 0) {
					cloudPoints[user][nIndex].x = nX;
					cloudPoints[user][nIndex].y = nY;
					cloudPoints[user][nIndex].z = pDepth[nIndex];
					cloudColors[user][nIndex].r = hasImageGenerator ? pColor[nIndex].nRed : 255;
					cloudColors[user][nIndex].g = hasImageGenerator ? pColor[nIndex].nGreen : 255;
					cloudColors[user][nIndex].b = hasImageGenerator ? pColor[nIndex].nBlue : 255;
					cloudColors[user][nIndex].a = 255;
				} else {
					cloudPoints[user][nIndex].z = 0;	// behaves a bit wackily (you need to ignore z == 0 data for userID > 0...)
				}

			}
		
		}
	}
}

ofPoint ofxUserGenerator::getWorldCoordinateAt(int x, int y, int userID) {
	
	return cloudPoints[userID][y * width + x];
	
}

ofColor ofxUserGenerator::getWorldColorAt(int x, int y, int userID) {
	
	return cloudColors[userID][y * width + x];
	
}

//--------------------------------------------------------------
void ofxUserGenerator::setSmoothing(float smooth) {
	if (smooth > 0.0f && smooth < 1.0f) {
		smoothing_factor = smooth;
		if (user_generator.IsValid()) {
			user_generator.GetSkeletonCap().SetSmoothing(smooth);
		}
	}
}

//--------------------------------------------------------------
float ofxUserGenerator::getSmoothing() {
	return smoothing_factor;
}

//----------------------------------------
int ofxUserGenerator::getWidth() {
	return width;
}

//----------------------------------------
int ofxUserGenerator::getHeight() {
	return height;
}

//----------------------------------------
void ofxUserGenerator::startTracking(XnUserID nID) {
	user_generator.GetSkeletonCap().StartTracking(nID);
}

//----------------------------------------
bool ofxUserGenerator::needsPoseForCalibration() {
	return needs_pose;
}

// Get a ref to the xn::UserGenerator object.
//----------------------------------------
xn::UserGenerator& ofxUserGenerator::getXnUserGenerator() {
	return user_generator;
}
