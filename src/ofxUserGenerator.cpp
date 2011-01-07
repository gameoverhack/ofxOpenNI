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
,num_users(15)
{

}

void ofxUserGenerator::startPoseDetection(XnUserID nID) {
	user_generator.GetPoseDetectionCap().StartPoseDetection(calibration_pose, nID);
}

void ofxUserGenerator::stopPoseDetection(XnUserID nID) {
	user_generator.GetPoseDetectionCap().StopPoseDetection(nID);
}

void ofxUserGenerator::requestCalibration(XnUserID nID) {
	user_generator.GetSkeletonCap().RequestCalibration(nID, TRUE);
}

bool ofxUserGenerator::setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator) {
	depth_generator = pDepthGenerator;
	context			= pContext;
	XnStatus result = XN_STATUS_OK;
	
	// check if the USER generator exists.
	result = context
				->getXnContext()
				->FindExistingNode(XN_NODE_TYPE_USER, user_generator);
	CHECK_RC(result, "Find user generator");
	if(result != XN_STATUS_OK) {
		return false;
	}
	
	// create user generator.
	result = user_generator.Create(*context->getXnContext());
	CHECK_RC(result, "Create user generator");
	if(result != XN_STATUS_OK) {
		return false;
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
	
	// pre-generate the tracked users.
	tracked_users.reserve(num_users);
	for(int i = 0; i < num_users; ++i) {
		ofxTrackedUser* tracked_user = new ofxTrackedUser(this, pDepthGenerator);
		tracked_users.push_back(tracked_user);
	}
	return true;
}

void ofxUserGenerator::drawUsers() {
	for(int i = 0;  i < found_users; ++i) {
		drawUser(i);
	}
}

void ofxUserGenerator::drawUser(int nUserNum) {
	if(nUserNum > tracked_users.size()-1)
		return;
	tracked_users.at(nUserNum)->updateBonePositions();
	tracked_users.at(nUserNum)->debugDraw();
}

xn::UserGenerator* ofxUserGenerator::getXnUserGenerator() {
	return &user_generator;
}

ofxTrackedUser* ofxUserGenerator::getTrackedUser(int nUserNum) {
	if(nUserNum > found_users-1)
		return NULL;
	return tracked_users.at(nUserNum);
}

void ofxUserGenerator::update() {
	XnUserID* users = new XnUserID[num_users];
	user_generator.GetUsers(users, found_users);
	for(int i = 0; i < found_users; ++i) {
		if(user_generator.GetSkeletonCap().IsTracking(users[i])) {	
			tracked_users.at(i)->id = users[i];
			tracked_users.at(i)->updateBonePositions();
		}
	}
	found_users = num_users;
	delete[] users;
}

void ofxUserGenerator::draw() {
	drawUsers();
}

void ofxUserGenerator::startTracking(XnUserID nID) {
	user_generator.GetSkeletonCap().StartTracking(nID);
}

bool ofxUserGenerator::needsPoseForCalibration() {
	return needs_pose;
}