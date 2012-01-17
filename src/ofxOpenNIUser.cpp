/*
 * ofxOpenNISkeleton.cpp
 *
 *  Created on: 12/10/2011
 *      Author: arturo
 */

#include "ofxOpenNIUser.h"

//--------------------------------------------------------------
ofxOpenNIUser::ofxOpenNIUser(){
    
    cloudPointDrawSize = 2;
    cloudPointResolution = 2;
    
    bUseMaskPixels = false;
    bUseMaskTexture = false;
    bIsTracking = false;
    
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

//--------------------------------------------------------------
void ofxOpenNIUser::drawSkeleton() {
    ofPushStyle();
    ofPushMatrix();
	for(int i=0;i<NumLimbs;i++){
        ofSetColor(255, 0, 0);
		limbs[i].draw();
	}
    ofPopMatrix();
    ofPopStyle();
	//ofDrawBitmapString(ofToString((int)id), neck.position[0].X + 10, neck.position[0].Y);
}

//--------------------------------------------------------------
void ofxOpenNIUser::drawPointCloud(){
    ofPushStyle();
    ofPushMatrix();
    glPointSize(cloudPointDrawSize);
    glEnable(GL_DEPTH_TEST);
    pointCloud.drawVertices();
    glDisable(GL_DEPTH_TEST);
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofxOpenNIUser::drawMask(){
    if (bUseMaskTexture){
        ofPushStyle();
        ofPushMatrix();
        maskTexture.draw(0, 0, maskPixels.getWidth(), maskPixels.getHeight());
        ofPopMatrix();
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofxOpenNIUser::setUseMaskTexture(bool b){
    bUseMaskTexture = b;
    if (bUseMaskTexture) {
        bUseMaskPixels = true; // have to use pixels to use the texture!
    } else {
        //if (bUseMaskPixels) // TODO: warn or set false?;
    }
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getUseMaskTexture(){
    return bUseMaskTexture;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setUseMaskPixels(bool b){
    bUseMaskPixels = b;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getUseMaskPixels(){
    return bUseMaskPixels;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setUsePointCloud(bool b){
    bUsePointCloud = b;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getUsePointCloud(){
    return bUsePointCloud;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setCloudPointDrawSize(int size){
    // this is the size of the points when drawing
    cloudPointDrawSize = size;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getCloudPointDrawSize(){
    return cloudPointDrawSize;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setCloudPointResolution(int resolution){
    // this is the step size when calculating (lower is higher res - 1 is the highest)
    cloudPointResolution = resolution;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getCloudPointResolution(){
    return cloudPointResolution;
}

//--------------------------------------------------------------
ofxOpenNILimb & ofxOpenNIUser::getLimb(Limb limb){
	return limbs[limb];
}

//--------------------------------------------------------------
int ofxOpenNIUser::getNumLimbs(){
	return NumLimbs;
}

//--------------------------------------------------------------
ofPoint & ofxOpenNIUser::getCenter(){
    return center;
}

//--------------------------------------------------------------
ofMesh & ofxOpenNIUser::getPointCloud(){
    return pointCloud;
}

//--------------------------------------------------------------
ofPixels & ofxOpenNIUser::getMaskPixels(){
    return maskPixels;
}

//--------------------------------------------------------------
ofTexture & ofxOpenNIUser::getMaskTextureReference(){
    return maskTexture;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getID(){
    return id;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::isTracking(){
    return bIsTracking;
}