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
	num_users = MAX_NUMBER_USERS;
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
	
	// store context and generator references
	context			= pContext;
	context->getDepthGenerator(&depth_generator);
	context->getImageGenerator(&image_generator);
	
	XnStatus result = XN_STATUS_OK;
	
	// get map_mode so we can setup width and height vars from depth gen size
	XnMapOutputMode map_mode; 
	depth_generator.GetMapOutputMode(map_mode);
	
	width = map_mode.nXRes;
	height = map_mode.nYRes;
	
	maskPixels = new unsigned char[width * height];
	
	// check if the USER generator exists.
	if(!context->getUserGenerator(&user_generator)) {
		
		// if one doesn't exist then create user generator.
		result = user_generator.Create(context->getXnContext());
		SHOW_RC(result, "Create user generator");
		
		if(result != XN_STATUS_OK) {
			return false;
		}
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
	
	// needs this to allow skeleton tracking when using pre-recorded .oni
	// as otherwise the image/depth nodes play but are not generating callbacks
	if (context->isUsingRecording()) {
		result = context->getXnContext().StartGeneratingAll();
		CHECK_RC(result, "StartGenerating");
	}

	// pre-generate the tracked users.
	for(int i = 0; i < num_users; ++i) {
		printf("Creting user: %i\n", i);
		ofxTrackedUser* tracked_user = new ofxTrackedUser(context);
		tracked_users[i] = tracked_user;
	}

	return true;
}


// Draw a specific user (start counting at 0)
//----------------------------------------
void ofxUserGenerator::drawUser(int nUserNum) {
	if(nUserNum > MAX_NUMBER_USERS - 1)
		return;
	tracked_users[nUserNum]->updateBonePositions();
	tracked_users[nUserNum]->debugDraw();
}

// Draw all the found users.
//----------------------------------------
void ofxUserGenerator::draw() {
	
	// show green/red circle if any one is found
	if (found_users > 0) {
		
		// draw all the users
		for(int i = 0;  i < found_users; ++i) {
			drawUser(i);
		}
		
		glColor3f(0, 1.0, 0);
	} else glColor3f(1, 1, 0);
	
	ofCircle(10,10,10);
	
	// reset to white for simplicity
	glColor3f(1, 1, 1);
	
}

void ofxUserGenerator::drawUserMasks(int x, int y) {
	
	for(int i = 0;  i < found_users; ++i) {
		updateUserMask(i);	// TODO: put into an update cycle instead of here
		maskImage[i].draw(x, y);
	}
	
}

// Get a tracked user.
//----------------------------------------
ofxTrackedUser* ofxUserGenerator::getTrackedUser(int nUserNum) {
	
	if(nUserNum > found_users - 1)
		return NULL;
	return tracked_users[nUserNum];
	
}

// Get number of tracked users
int ofxUserGenerator::getNumberOfTrackedUsers() {
	return found_users;
}

// Update the tracked users, should be called each frame
//----------------------------------------
void ofxUserGenerator::update() {
	
	found_users = num_users;
	
	XnUserID* users = new XnUserID[num_users];
	user_generator.GetUsers(users, found_users);
	
	for(int i = 0; i < found_users; ++i) {
		if(user_generator.GetSkeletonCap().IsTracking(users[i])) {	
			tracked_users[i]->id = users[i];
			tracked_users[i]->updateBonePositions();
		}
	}
	
	delete [] users;
}

void ofxUserGenerator::updateUserMask(int userID) {
	
	const XnLabel* pLabels = NULL;
	xn::SceneMetaData smd;
	
	if (user_generator.GetUserPixels(userID, smd) == XN_STATUS_OK) {
		pLabels = smd.Data();
	}
	
	for (int i =0 ; i < width * height; i++, pLabels++) {
		
		if (*pLabels != 0) {
			maskPixels[i] = 255;
		} else maskPixels[i] = 0;
		
	}

	if(maskImage[userID].width == 0 && maskImage[userID].height == 0) {
		maskImage[userID].allocate(width, height);
	}
	
	maskImage[userID].setFromPixels(maskPixels, width, height);

	maskImage[userID].erode_3x3();
	maskImage[userID].erode_3x3();
}

void ofxUserGenerator::setPointCloudRotation(int _x) {
	cloudPointRotationY = _x;
}

//----------------------------------------
void ofxUserGenerator::drawPointCloud(bool showBackground, int cloudPointSize) {

	float fValueH = 0;
	
	xn::DepthMetaData dm;
	xn::ImageMetaData im;
	
	depth_generator.GetMetaData(dm);
	image_generator.GetMetaData(im);
	
	const XnDepthPixel* pDepth = dm.Data();
	const XnRGB24Pixel* pColor = im.RGB24Data();

	glPushMatrix();

	ofRotateY(cloudPointRotationY);
	
	glPointSize (cloudPointSize);
	glBegin(GL_POINTS);
	
	XnBool bLabelsExists = false;
	const XnLabel* pLabels = NULL;
	xn::SceneMetaData smd;
	
	if (user_generator.GetUserPixels(0, smd) == XN_STATUS_OK) {	// TODO: change so you can draw just one user, etc - currently draws all users	
		bLabelsExists = TRUE;
		pLabels = smd.Data();
	}
	
	XnUInt32 nIndex = 0;
	for (XnUInt16 nY = 0; nY < height; nY++) {
		
		for (XnUInt16 nX = 0; nX < width; nX++, nIndex++, pLabels++) {
			
			fValueH = pDepth[nIndex];
			
			if (pDepth[nIndex] == 0) continue;
			
			if (bLabelsExists) {
				if (*pLabels == 0) {
					if (!showBackground) continue;
				}
			}
			else if (!showBackground) continue;
			
			glColor3f(float(pColor[nIndex].nRed/127.0), float(pColor[nIndex].nBlue/127.0), float(pColor[nIndex].nGreen/127.0));
			
			// TODO: originally this used Real world to projective calls, 
			// but that slowed things down too much for me...need to check 
			// ofxKinect code and original to make this more better ;-)
			
			XnPoint3D point = xnCreatePoint3D(nX, nY, fValueH);
			glVertex3f(point.X, point.Y, point.Z);
			
		}
	}
	glEnd();

	glPopMatrix();
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