/*
 * ofxOpenNISkeleton.cpp
 *
 *  Created on: 12/10/2011
 *      Author: arturo
 */

#include "ofxOpenNIUser.h"

//----------------------------------------
ofxOpenNIUser::ofxOpenNIUser(){
	limbs.resize(NumLimbs);

	limbs[Neck].set(XN_SKEL_HEAD, XN_SKEL_NECK);

	// left arm + shoulder
	limbs[LeftShoulder].set(XN_SKEL_NECK, XN_SKEL_LEFT_SHOULDER);
	limbs[LeftUpperArm].set(XN_SKEL_LEFT_SHOULDER, XN_SKEL_LEFT_ELBOW);
	limbs[LeftLowerArm].set(XN_SKEL_LEFT_ELBOW, XN_SKEL_LEFT_HAND);

	// right arm + shoulder
	limbs[RightShoulder].set(XN_SKEL_NECK, XN_SKEL_RIGHT_SHOULDER);
	limbs[RightUpperArm].set(XN_SKEL_RIGHT_SHOULDER, XN_SKEL_RIGHT_ELBOW);
	limbs[RightLowerArm].set(XN_SKEL_RIGHT_ELBOW, XN_SKEL_RIGHT_HAND);

	// upper torso
	limbs[LeftUpperTorso].set(XN_SKEL_LEFT_SHOULDER, XN_SKEL_TORSO);
	limbs[RightUpperTorso].set(XN_SKEL_RIGHT_SHOULDER, XN_SKEL_TORSO);

	// left lower torso + leg
	limbs[LeftLowerTorso].set(XN_SKEL_TORSO, XN_SKEL_LEFT_HIP);
	limbs[LeftUpperLeg].set(XN_SKEL_LEFT_HIP, XN_SKEL_LEFT_KNEE);
	limbs[LeftLowerLeg].set(XN_SKEL_LEFT_KNEE, XN_SKEL_LEFT_FOOT);

	// right lower torso + leg
	limbs[RightLowerTorso].set(XN_SKEL_TORSO, XN_SKEL_RIGHT_HIP);
	limbs[RightUpperLeg].set(XN_SKEL_RIGHT_HIP, XN_SKEL_RIGHT_KNEE);
	limbs[RightLowerLeg].set(XN_SKEL_RIGHT_KNEE, XN_SKEL_RIGHT_FOOT);

	limbs[Hip].set(XN_SKEL_LEFT_HIP, XN_SKEL_RIGHT_HIP);
}

//----------------------------------------
void ofxOpenNIUser::debugDraw() {

	for(int i=0;i<NumLimbs;i++){
		limbs[i].debugDraw();
	}

	//ofDrawBitmapString(ofToString((int)id), neck.position[0].X + 10, neck.position[0].Y);
}

//----------------------------------------
ofxOpenNILimb & ofxOpenNIUser::getLimb(Limb limb){
	return limbs[limb];
}

//----------------------------------------
int ofxOpenNIUser::getNumLimbs(){
	return NumLimbs;
}

