#include "ofxTrackedUser.h"

ofxTrackedUser::ofxTrackedUser(ofxOpenNIContext* pContext) 
:neck(XN_SKEL_HEAD, XN_SKEL_NECK)

// left arm + shoulder
,left_shoulder(XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER)
,left_upper_arm(XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW)
,left_lower_arm(XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND)

// right arm + shoulder
,right_shoulder(XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER)
,right_upper_arm(XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW)
,right_lower_arm(XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND)

// upper torso
,left_upper_torso(XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO)
,right_upper_torso(XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO)

// left lower torso + leg
,left_lower_torso(XN_SKEL_TORSO, XN_SKEL_LEFT_HIP)
,left_upper_leg(XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE)
,left_lower_leg(XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT)

// right lower torso + leg
,right_lower_torso(XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP)
,right_upper_leg(XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE)
,right_lower_leg(XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT)

,hip(XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP)
{
	context = pContext;
	context->getDepthGenerator(&depth_generator);
	context->getUserGenerator(&user_generator);
}

void ofxTrackedUser::updateBonePositions() {
	
	updateLimb(neck);
	
	// left arm + shoulder
	updateLimb(left_shoulder);
	updateLimb(left_upper_arm);
	updateLimb(left_lower_arm);
	
	// right arm + shoulder
	updateLimb(right_shoulder);
	updateLimb(right_upper_arm);
	updateLimb(right_lower_arm);
	
	// upper torso
	updateLimb(left_upper_torso);
	updateLimb(right_upper_torso);
	
	// left lower torso + leg
	updateLimb(left_lower_torso);
	updateLimb(left_upper_leg);
	updateLimb(left_lower_leg);
	
	// right lower torso + leg
	updateLimb(right_lower_torso);
	updateLimb(right_upper_leg);
	updateLimb(right_lower_leg);

	updateLimb(hip);	
}

void ofxTrackedUser::updateLimb(ofxLimb& rLimb) {
	
	if(!user_generator.GetSkeletonCap().IsTracking(id)) {
		//printf("Not tracking this user: %d\n", id);
		return;
	}
	
	XnSkeletonJointPosition a,b;
	user_generator.GetSkeletonCap().GetSkeletonJointPosition(id, rLimb.start_joint, a);
	user_generator.GetSkeletonCap().GetSkeletonJointPosition(id, rLimb.end_joint, b);
	if(a.fConfidence < 0.3f || b.fConfidence < 0.3f) {
		rLimb.found = false; 
		return;
	}
	
	rLimb.found = true;
	rLimb.position[0] = a.position;
	rLimb.position[1] = b.position;
	
	depth_generator.ConvertRealWorldToProjective(2, rLimb.position, rLimb.position);
	
}

void ofxTrackedUser::debugDraw() {
	
	neck.debugDraw();
	
	// left arm + shoulder
	left_shoulder.debugDraw();
	left_upper_arm.debugDraw();
	left_lower_arm.debugDraw();
	
	// right arm + shoulder
	right_shoulder.debugDraw();
	right_upper_arm.debugDraw();
	right_lower_arm.debugDraw();
	
	// upper torso
	left_upper_torso.debugDraw();
	right_upper_torso.debugDraw();
	
	// left lower torso + leg
	left_lower_torso.debugDraw();
	left_upper_leg.debugDraw();
	left_lower_leg.debugDraw();

	// right lower torso + leg
	right_lower_torso.debugDraw();
	right_upper_leg.debugDraw();
	right_lower_leg.debugDraw();
	
	hip.debugDraw();

	ofDrawBitmapString(ofToString((int)id), neck.position[0].X + 10, neck.position[0].Y);
}
