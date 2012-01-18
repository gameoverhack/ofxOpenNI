/*
 * ofxOpenNIUser.cpp
 *
 * Copyright 2011 (c) Matthew Gingold [gameover] http://gingold.com.au
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

#include "ofxOpenNIUser.h"

//--------------------------------------------------------------
ofxOpenNIUser::ofxOpenNIUser(){
    
    cloudPointDrawSize = 2;
    cloudPointResolution = 2;
    
    limbDetectionConfidence =  0.3f;
    
    bUseMaskPixels = false;
    bUseMaskTexture = false;
    bUsePointCloud = false;
    bIsFound = false;
    bIsTracking = false;
    bIsSkeleton = false;
    bIsCalibrating = false;
    bUseAutoCalibration = true;
    bNewPixels = false;
    
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
	for(int i = 0; i < NumLimbs; i++){
        ofSetColor(255, 0, 0);
		limbs[i].draw();
	}
    ofPopMatrix();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofxOpenNIUser::drawPointCloud(){
    ofPushStyle();
    ofPushMatrix();
    glPointSize(cloudPointDrawSize);
    glEnable(GL_DEPTH_TEST);
    pointCloud[1].drawVertices();
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
void ofxOpenNIUser::setUseAutoCalibration(bool b){
    bUseAutoCalibration = b;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getUseAutoCalibration(){
    return bUseAutoCalibration;
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
    return pointCloud[1];
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
void ofxOpenNIUser::setLimbDetectionConfidence(float level){
    limbDetectionConfidence = level;
}

//--------------------------------------------------------------
float ofxOpenNIUser::getLimbDetectionConfidence(){
    return limbDetectionConfidence;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getID(){
    return id;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::isFound(){
    return bIsFound;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::isTracking(){
    return bIsTracking;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::isSkeleton(){
    return bIsSkeleton;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::isCalibrating(){
    return bIsCalibrating;
}

//--------------------------------------------------------------
string ofxOpenNIUser::getDebugInfo(){
    ostringstream info;
    info << "userID: " << ofToString(id) << " ";
    info << "autoCalibrate: " << boolToString(bUseAutoCalibration) << " ";
    info << "isFound: " << boolToString(bIsFound) << " ";
    info << "isTracking: " << boolToString(bIsTracking) << " ";
    info << "isSkeleton: " << boolToString(bIsSkeleton) << " ";
    info << "isCalibrating: " << boolToString(bIsCalibrating) << endl;
    info << "useMaskPixels: " << boolToString(bUseMaskPixels) << " ";
    info << "useMaskTexture: " << boolToString(bUseMaskTexture) << " ";
    info << "usePointCloud: " << boolToString(bUsePointCloud) << " ";
    if (bUsePointCloud) {
        info << "cloudPointDrawSize: " << ofToString(cloudPointDrawSize) << " ";
        info << "cloudPointResolution: " << ofToString(cloudPointResolution) << " ";
    }
    return info.str();
}