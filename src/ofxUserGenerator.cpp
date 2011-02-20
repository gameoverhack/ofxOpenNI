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
ofxUserGenerator::ofxUserGenerator() 
:needs_pose(false)
,num_users(MAX_NUMBER_USERS)
,is_initialized(false)
{	
	found_user = false;
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
bool ofxUserGenerator::setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator, ofxImageGenerator* pImageGenerator) {
	if(!pContext->isInitialized()) {
		return false;
	}
	
	image_generator = pImageGenerator;
	depth_generator = pDepthGenerator;
	context			= pContext;
	XnStatus result = XN_STATUS_OK;
	
	xn::DepthMetaData dm;
	
	depth_generator->getXnDepthGenerator().GetMetaData(dm);
	
	width = dm.XRes();
	height = dm.YRes();
	
	maskPixels = new unsigned char[width * height];
	
	// check if the USER generator exists.
	result = context
				->getXnContext()
				.FindExistingNode(XN_NODE_TYPE_USER, user_generator);
	SHOW_RC(result, "Find user generator");
	if(result != XN_STATUS_OK) {
		// create user generator.
		result = user_generator.Create(context->getXnContext());
		SHOW_RC(result, "Create user generator");
		if(result != XN_STATUS_OK) {
			return false;
		}
	}	
	
	// register user callbacks/
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
	
	// added by gameover (m gingold) -> needs this to allow skeleton tracking when using pre-recorded .oni
	if (context->isUsingRecording()) {
		result = context->getXnContext().StartGeneratingAll();
		CHECK_RC(result, "StartGenerating");
	}

	
	// pre-generate the tracked users.
	tracked_users.reserve(num_users);
	for(int i = 0; i < num_users; ++i) {
		ofxTrackedUser* tracked_user = new ofxTrackedUser(this, pDepthGenerator);
		tracked_users.push_back(tracked_user);
	}
	is_initialized = true;
	return true;
}


// Draw all the found users.
//----------------------------------------
void ofxUserGenerator::drawUsers() {
	for(int i = 0;  i < num_users; ++i) {
		if (tracked_users.at(i)->is_tracked) {
			drawUser(i);
		}
	}
}


// Draw a specific user (start counting at 0)
//----------------------------------------
void ofxUserGenerator::drawUser(int nUserNum) {
	if(nUserNum > tracked_users.size()-1)
		return;
	tracked_users.at(nUserNum)->updateBonePositions();
	tracked_users.at(nUserNum)->debugDraw();
}


// Get a ref to the xn::UserGenerator object.
//----------------------------------------
xn::UserGenerator& ofxUserGenerator::getXnUserGenerator() {
	return user_generator;
}


// Get a tracked user.
//----------------------------------------
ofxTrackedUser* ofxUserGenerator::getTrackedUser(int nUserNum) {
	
	ofxTrackedUser* found_user = NULL;
	try {
		found_user = tracked_users.at(nUserNum);
	}
	catch( std::out_of_range& rEx) {
		return NULL;
	}
	return found_user;
}

std::vector<ofxTrackedUser*> ofxUserGenerator::getTrackedUsers() {
	std::vector<ofxTrackedUser*> found;
	std::vector<ofxTrackedUser*>::iterator it = tracked_users.begin();
	while(it != tracked_users.end()) {
		if( (*it)->is_tracked) {
			found.push_back(*it);
		}			
		++it;
	}
	return found;
}


// Update the tracked users, should be called each frame
//----------------------------------------
void ofxUserGenerator::update() {
	if(!is_initialized) {
		return;
	}
	
	// unset
	std::vector<ofxTrackedUser*>::iterator it = tracked_users.begin();
	while(it != tracked_users.end()) {
		(*it)->is_tracked = false;
		++it;
	}
	
	found_user = false;
	XnUserID* users = new XnUserID[num_users];
	user_generator.GetUsers(users, found_users);
	for(int i = 0; i < found_users; ++i) {
		if(user_generator.GetSkeletonCap().IsTracking(users[i])) {	
			found_user = true;
			tracked_users.at(i)->is_tracked = true;
			tracked_users.at(i)->id = users[i];
			tracked_users.at(i)->updateBonePositions();
		} else {
			tracked_users.at(i)->is_tracked = false;
		}

	}
	found_users = num_users;
	delete[] users;
}

// Draw all users.
//----------------------------------------
void ofxUserGenerator::draw() {
	if(!is_initialized) {
		return;
	}

	drawUsers();
	if(!found_user) {
		glColor3f(1.0, 0, 0);
	}
	else {
		glColor3f(0.0, 1, 0);
	}
	ofCircle(10,10,10);
	
	// reset to white for simplicity elsewhere
	glColor3f(1, 1, 1);
}

void ofxUserGenerator::drawUserMasks(int x, int y) {
	for(int i = 0;  i < num_users; ++i) {
		if (tracked_users.at(i)->is_tracked) {
			updateUserMask(i);	// TODO: put into an update cycle instead of here
			maskImage[i].draw(x, y);
		}
	}
}

void ofxUserGenerator::updateUserMask(int userID) {
	
	const XnLabel* pLabels = NULL;
	xn::SceneMetaData smd;
	
	if (user_generator.GetUserPixels(userID, smd) == XN_STATUS_OK)
	{
		pLabels = smd.Data();
	}
	
	for (int i =0 ; i < width * height; i++, pLabels++) {
		
		if (*pLabels != 0) {
			maskPixels[i] = 255;
		} else {
            maskPixels[i] = 0;
		}
		
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
	
	depth_generator->getXnDepthGenerator().GetMetaData(dm);
	image_generator->getXnImageGenerator().GetMetaData(im);
	
	const XnDepthPixel* pDepth = dm.Data();
	const XnRGB24Pixel* pColor = im.RGB24Data();

	glPushMatrix();

	ofRotateY(cloudPointRotationY);
	
	glPointSize (cloudPointSize);
	glBegin(GL_POINTS);
	
	XnBool bLabelsExists = false;
	const XnLabel* pLabels = NULL;
	xn::SceneMetaData smd;
	
	if (user_generator.GetUserPixels(0, smd) == XN_STATUS_OK)	// TODO: track all users, currently only drawing point cloud for first user
	{
		bLabelsExists = TRUE;
		pLabels = smd.Data();
	}
	
	XnUInt32 nIndex = 0;
	for (XnUInt16 nY = 0; nY < height; nY++)
	{
		for (XnUInt16 nX = 0; nX < width; nX++, nIndex++, pLabels++)
		{
			fValueH = pDepth[nIndex];
			if (pDepth[nIndex] == 0)
				continue;
			
			if (bLabelsExists)
			{
				if (*pLabels == 0)
				{
					if (!showBackground)
					{
						continue;
					}
				}
			}
			else if (!showBackground)
			{
				continue;
			}
			
			glColor3f(float(pColor[nIndex].nRed/127.0), float(pColor[nIndex].nBlue/127.0), float(pColor[nIndex].nGreen/127.0));
			
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