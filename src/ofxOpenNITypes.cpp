/*
 * ofxOpenNITypes.cpp
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

#include "ofxOpenNITypes.h"

//--------------------------------------------------------------
ofxOpenNIUser::ofxOpenNIUser(){
    
    pointCloudDrawSize = 2;
    pointCloudResolution = 2;
    
    confidenceThreshold =  0.5f;
    
    bUseMaskPixels = false;
    bUseMaskTexture = false;
    bUsePointCloud = false;
    bIsFound = false;
    bIsTracking = false;
    bIsSkeleton = false;
    bIsCalibrating = false;
    bUseAutoCalibration = true;
    bNewPixels = false;
    bUseSkeleton = true;
    bForceReset = false;
    bForceRestart = false;
    bUseOrientation = false;
    
    userPixels = NULL;
    
    forcedResetTimeout = 1000;
    resetCount = 0;
    
}

//--------------------------------------------------------------
ofxOpenNIUser::~ofxOpenNIUser(){
    clear();
}

void ofxOpenNIUser::setup(){
    
    backPointCloud = &pointCloud[0];
	currentPointCloud = &pointCloud[1];
    
    joints.resize(JOINT_COUNT);

    // head
    joints[JOINT_HEAD].set(XN_SKEL_HEAD, confidenceThreshold);
    joints[JOINT_NECK].set(XN_SKEL_NECK, confidenceThreshold);
    
    // left arm
    joints[JOINT_LEFT_SHOULDER].set(XN_SKEL_LEFT_SHOULDER, confidenceThreshold);
    joints[JOINT_LEFT_ELBOW].set(XN_SKEL_LEFT_ELBOW, confidenceThreshold);
    joints[JOINT_LEFT_HAND].set(XN_SKEL_LEFT_HAND, confidenceThreshold);
    
    // right arm
    joints[JOINT_RIGHT_SHOULDER].set(XN_SKEL_RIGHT_SHOULDER, confidenceThreshold);
    joints[JOINT_RIGHT_ELBOW].set(XN_SKEL_RIGHT_ELBOW, confidenceThreshold);
    joints[JOINT_RIGHT_HAND].set(XN_SKEL_RIGHT_HAND, confidenceThreshold);
    
    // torso
    joints[JOINT_TORSO].set(XN_SKEL_TORSO, confidenceThreshold);
    
    // left leg
    joints[JOINT_LEFT_HIP].set(XN_SKEL_LEFT_HIP, confidenceThreshold);
    joints[JOINT_LEFT_KNEE].set(XN_SKEL_LEFT_KNEE, confidenceThreshold);
    joints[JOINT_LEFT_FOOT].set(XN_SKEL_LEFT_FOOT, confidenceThreshold);
    
    // right leg
    joints[JOINT_RIGHT_HIP].set(XN_SKEL_RIGHT_HIP, confidenceThreshold);
    joints[JOINT_RIGHT_KNEE].set(XN_SKEL_RIGHT_KNEE, confidenceThreshold);
    joints[JOINT_RIGHT_FOOT].set(XN_SKEL_RIGHT_FOOT, confidenceThreshold);
    
    joints[JOINT_HEAD].setParent(joints[JOINT_NECK]);
    joints[JOINT_NECK].setParent(joints[JOINT_TORSO]);
    joints[JOINT_RIGHT_SHOULDER].setParent(joints[JOINT_NECK]);
    joints[JOINT_RIGHT_ELBOW].setParent(joints[JOINT_RIGHT_SHOULDER]);
    joints[JOINT_RIGHT_HAND].setParent(joints[JOINT_RIGHT_ELBOW]);
    joints[JOINT_LEFT_SHOULDER].setParent(joints[JOINT_NECK]);
    joints[JOINT_LEFT_ELBOW].setParent(joints[JOINT_LEFT_SHOULDER]);
    joints[JOINT_LEFT_HAND].setParent(joints[JOINT_LEFT_ELBOW]);
    joints[JOINT_LEFT_HIP].setParent(joints[JOINT_TORSO]);
    joints[JOINT_LEFT_KNEE].setParent(joints[JOINT_LEFT_HIP]);
    joints[JOINT_LEFT_FOOT].setParent(joints[JOINT_LEFT_KNEE]);
    joints[JOINT_RIGHT_HIP].setParent(joints[JOINT_TORSO]);
    joints[JOINT_RIGHT_KNEE].setParent(joints[JOINT_RIGHT_HIP]);
    joints[JOINT_RIGHT_FOOT].setParent(joints[JOINT_RIGHT_KNEE]);
    
//    // SET INITIAL ORIENTATIONS OF JOINTS - NOT WORKING YET!!! 
//    ofQuaternion q;
//    q.set(0.0f, 0.0f, 0.0f, 1.0f); // IDENTITY
//    ofQuaternion q2;
//    ofVec3f xAxis,yAxis,zAxis;
//    
//    quaternianFromAngleAxis(q, ofDegToRad(90), ofVec3f(0,0,-1));
//    quaternionToAxes(q, xAxis, yAxis, zAxis);
//    quaternianFromAngleAxis(q2, ofDegToRad(90), xAxis);
//    joints[JOINT_LEFT_SHOULDER].setInitialOrientation(q*q2);
//    //setupBone("Humerus.L",q*q2);
//    
//    quaternianFromAngleAxis(q, ofDegToRad(90), ofVec3f(0,0,1));
//    quaternionToAxes(q, xAxis, yAxis, zAxis);
//    quaternianFromAngleAxis(q2, ofDegToRad(90), xAxis);
//    joints[JOINT_RIGHT_SHOULDER].setInitialOrientation(q*q2);
//    //setupBone("Humerus.R",q*q2);
//    
//    quaternianFromAngleAxis(q, ofDegToRad(90), ofVec3f(0,0,-1));	 
//    quaternianFromAngleAxis(q2, ofDegToRad(45), ofVec3f(0,-1,0));
//    joints[JOINT_LEFT_ELBOW].setInitialOrientation(q*q2);
//    //setupBone("Ulna.L",q*q2);
//    
//    quaternianFromAngleAxis(q, ofDegToRad(90), ofVec3f(0,0,1));
//    joints[JOINT_RIGHT_ELBOW].setInitialOrientation(q*q2);
//    //setupBone("Ulna.R",q*q2.Inverse());
//    
//    quaternianFromAngleAxis(q, ofDegToRad(180), ofVec3f(0,1,0));
//    joints[JOINT_TORSO].setInitialOrientation(q*q2);
//    //setupBone("Chest",q);
//    //setupBone("Stomach",q);
//    
//    quaternianFromAngleAxis(q, ofDegToRad(180), ofVec3f(1,0,0));	 	
//    quaternianFromAngleAxis(q2, ofDegToRad(180), ofVec3f(0,1,0));
//    joints[JOINT_LEFT_HIP].setInitialOrientation(q*q2);
//    joints[JOINT_LEFT_KNEE].setInitialOrientation(q*q2);
//    joints[JOINT_RIGHT_HIP].setInitialOrientation(q*q2);
//    joints[JOINT_RIGHT_KNEE].setInitialOrientation(q*q2);
//    //setupBone("Thigh.L",q*q2);
//    //setupBone("Thigh.R",q*q2);
//    //setupBone("Calf.L",q*q2);
//    //setupBone("Calf.R",q*q2);  
//    //setupBone("Root",Degree(0),Degree(0),Degree(0));
    
    // DONT REALLY NEED LIMBS ANYMORE BUT LEAVING THEM HERE
    // USE joint and joint.getParent() for start/end instead!
	limbs.resize(LIMB_COUNT);
    
    // head
	limbs[LIMB_NECK].set(joints[JOINT_HEAD], joints[JOINT_NECK]);
    
	// left arm + shoulder
	limbs[LIMB_LEFT_SHOULDER].set(joints[JOINT_NECK], joints[JOINT_LEFT_SHOULDER]);
	limbs[LIMB_LEFT_UPPER_ARM].set(joints[JOINT_LEFT_SHOULDER], joints[JOINT_LEFT_ELBOW]);
	limbs[LIMB_LEFT_LOWER_ARM].set(joints[JOINT_LEFT_ELBOW], joints[JOINT_LEFT_HAND]);
    
	// right arm + shoulder
	limbs[LIMB_RIGHT_SHOULDER].set(joints[JOINT_NECK], joints[JOINT_RIGHT_SHOULDER]);
	limbs[LIMB_RIGHT_UPPER_ARM].set(joints[JOINT_RIGHT_SHOULDER], joints[JOINT_RIGHT_ELBOW]);
	limbs[LIMB_RIGHT_LOWER_ARM].set(joints[JOINT_RIGHT_ELBOW], joints[JOINT_RIGHT_HAND]);
    
	// upper torso
	limbs[LIMB_LEFT_UPPER_TORSO].set(joints[JOINT_LEFT_SHOULDER], joints[JOINT_TORSO]);
	limbs[LIMB_RIGHT_UPPER_TORSO].set(joints[JOINT_RIGHT_SHOULDER], joints[JOINT_TORSO]);
    
	// left lower torso + leg
	limbs[LIMB_LEFT_LOWER_TORSO].set(joints[JOINT_TORSO], joints[JOINT_LEFT_HIP]);
	limbs[LIMB_LEFT_UPPER_LEG].set(joints[JOINT_LEFT_HIP], joints[JOINT_LEFT_KNEE]);
	limbs[LIMB_LEFT_LOWER_LEG].set(joints[JOINT_LEFT_KNEE], joints[JOINT_LEFT_FOOT]);
    
	// right lower torso + leg
	limbs[LIMB_RIGHT_LOWER_TORSO].set(joints[JOINT_TORSO], joints[JOINT_RIGHT_HIP]);
	limbs[LIMB_RIGHT_UPPER_LEG].set(joints[JOINT_RIGHT_HIP], joints[JOINT_RIGHT_KNEE]);
	limbs[LIMB_RIGHT_LOWER_LEG].set(joints[JOINT_RIGHT_KNEE], joints[JOINT_RIGHT_FOOT]);
    
	limbs[LIMB_PELVIS].set(joints[JOINT_LEFT_HIP], joints[JOINT_RIGHT_HIP]);
    
    setUseOrientation(bUseOrientation);
}

void ofxOpenNIUser::clear(){
    //delete [] userPixels;
    joints.clear();
    limbs.clear();
    pointCloud[0].clear();
    pointCloud[1].clear();
    maskPixels.clear();
    maskTexture.clear();
}

//--------------------------------------------------------------
void ofxOpenNIUser::drawSkeleton() {
    ofPushStyle();
    // DON'T NEED TO DRAW LIMBS ANYMORE!
//	for(int i = 0; i < limbs.size(); i++){
//		limbs[i].draw();
//	}
    for(int i = 0; i < joints.size(); i++){
		joints[i].draw();
	}
    ofPopStyle();
}

//--------------------------------------------------------------
void ofxOpenNIUser::drawPointCloud(){
    ofPushStyle();
    glPointSize(pointCloudDrawSize);
    glEnable(GL_DEPTH_TEST);
    currentPointCloud->drawVertices();
    glDisable(GL_DEPTH_TEST);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofxOpenNIUser::drawMask(){
    if (bUseMaskTexture){
        ofPushStyle();
        maskTexture.draw(0, 0, maskPixels.getWidth(), maskPixels.getHeight());
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
    if(bUseMaskTexture) bUseMaskPixels = true;
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
    return (bUseMaskPixels || bUseMaskTexture);
}

//--------------------------------------------------------------
void ofxOpenNIUser::setUseSkeleton(bool b){
    bUseSkeleton = b;
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getUseSkeleton(){
    return bUseSkeleton;
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
void ofxOpenNIUser::setPointCloudDrawSize(int size){
    // this is the size of the points when drawing
    pointCloudDrawSize = size;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getPointCloudDrawSize(){
    return pointCloudDrawSize;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setPointCloudResolution(int resolution){
    // this is the step size when calculating (lower is higher res - 1 is the highest)
    pointCloudResolution = resolution;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getPointCloudResolution(){
    return pointCloudResolution;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setUseOrientation(bool b){
    bUseOrientation = b;
    for(int j = 0; j < getNumJoints(); j++){
        ofxOpenNIJoint & joint = getJoint((Joint)j);
        joint.setUseOrientation(b);
    }
    
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getUseOrientation(){
    return bUseOrientation;
}

//--------------------------------------------------------------
ofxOpenNILimb & ofxOpenNIUser::getLimb(Limb limb){
	return limbs[limb];
}

//--------------------------------------------------------------
int ofxOpenNIUser::getNumLimbs(){
	return limbs.size();
}

//--------------------------------------------------------------
ofxOpenNIJoint & ofxOpenNIUser::getJoint(Joint joint){
	return joints[joint];
}

//--------------------------------------------------------------
int ofxOpenNIUser::getNumJoints(){
	return joints.size();
}

//--------------------------------------------------------------
ofPoint & ofxOpenNIUser::getCenter(){
    return center;
}

//--------------------------------------------------------------
ofMesh & ofxOpenNIUser::getPointCloud(){
    return (*currentPointCloud);
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
void ofxOpenNIUser::setConfidenceThreshold(float level){
    confidenceThreshold = level;
    for(int i = 0; i < joints.size(); i++){
        joints[i].setConfidenceThreshold(confidenceThreshold);
    }
}

//--------------------------------------------------------------
float ofxOpenNIUser::getConfidenceThreshold(){
    return confidenceThreshold;
}

////--------------------------------------------------------------
void ofxOpenNIUser::setForceResetTimeout(int millis){
    forcedResetTimeout = millis;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getForceResetTimeout(){
    return forcedResetTimeout;
}

//--------------------------------------------------------------
void ofxOpenNIUser::setForceReset(bool useTimeout, bool forceImmediateRestart){
    bForceRestart = forceImmediateRestart;
    if(!useTimeout){
        bForceReset = true;
    }else{
        resetCount++;
        if(resetCount > forcedResetTimeout/25){
            resetCount = 0;
            bForceReset = true;
        }
    }
}

//--------------------------------------------------------------
bool ofxOpenNIUser::getForceReset(){
    return bForceReset;
}

//--------------------------------------------------------------
int ofxOpenNIUser::getXnID(){
    return XnID;
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
    info << "XnUserID: " << ofToString(XnID) << " ";
    info << "autoCalibrate: " << boolToString(bUseAutoCalibration) << " ";
    info << "isFound: " << boolToString(isFound()) << " ";
    info << "isTracking: " << boolToString(isTracking()) << " ";
    info << "isSkeleton: " << boolToString(isSkeleton()) << " ";
    info << "isCalibrating: " << boolToString(isCalibrating()) << endl;
    info << "useMaskPixels: " << boolToString(getUseMaskPixels()) << " ";
    info << "useMaskTexture: " << boolToString(getUseMaskTexture()) << " ";
    info << "usePointCloud: " << boolToString(getUsePointCloud()) << " ";
    if (bUsePointCloud) {
        info << "pointCloudDrawSize: " << ofToString(pointCloudDrawSize) << " ";
        info << "pointCloudResolution: " << ofToString(pointCloudResolution) << " ";
    }
    return info.str();
}



//--------------------------------------------------------------
ofxOpenNIDepthThreshold::ofxOpenNIDepthThreshold(){
    ofPoint lbn = ofPoint(0, 0, 0);
    ofPoint rtf = ofPoint(0, 0, MAXDEPTH);
    roi.set(lbn, rtf);
    pointCloudDrawSize = 2;
    pointCloudResolution = 2;
    bUsePointCloud = false;
    bUseMaskPixels = true;
    bUseMaskTexture = true;
    bUseDepthPixels = false;
    bUseDepthTexture = false;
    bNewPixels = false;
    bNewPointCloud = false;
}

//--------------------------------------------------------------
ofxOpenNIDepthThreshold::ofxOpenNIDepthThreshold(int _nearThreshold,
                                                 int _farThreshold,
                                                 bool _bUsePointCloud,
                                                 bool _bUseMaskPixels,
                                                 bool _bUseMaskTexture,
                                                 bool _bUseDepthPixels,
                                                 bool _bUseDepthTexture,
                                                 int _pointCloudDrawSize,
                                                 int _pointCloudResolution){
    set(_nearThreshold, _farThreshold, _bUsePointCloud, _bUseMaskPixels, _bUseMaskTexture, _bUseDepthPixels, _bUseDepthTexture, _pointCloudDrawSize, _pointCloudResolution);
}

//--------------------------------------------------------------
ofxOpenNIDepthThreshold::ofxOpenNIDepthThreshold(ofxOpenNIROI & _roi,
                                                 bool _bUsePointCloud,
                                                 bool _bUseMaskPixels,
                                                 bool _bUseMaskTexture,
                                                 bool _bUseDepthPixels,
                                                 bool _bUseDepthTexture,
                                                 int _pointCloudDrawSize,
                                                 int _pointCloudResolution){
    set(_roi, _bUsePointCloud, _bUseMaskPixels, _bUseMaskTexture, _bUseDepthPixels, _bUseDepthTexture,_pointCloudDrawSize, _pointCloudResolution);
}

void ofxOpenNIDepthThreshold::set(int _nearThreshold,
                                  int _farThreshold,
                                  bool _bUsePointCloud,
                                  bool _bUseMaskPixels,
                                  bool _bUseMaskTexture,
                                  bool _bUseDepthPixels,
                                  bool _bUseDepthTexture,
                                  int _pointCloudDrawSize,
                                  int _pointCloudResolution){
    ofPoint lbn = ofPoint(0, 0, _nearThreshold);
    ofPoint rtf = ofPoint(0, 0, _farThreshold);
    roi.set(lbn, rtf);
    bUseXY = false;
    pointCloudDrawSize = _pointCloudDrawSize;
    pointCloudResolution = _pointCloudResolution;
    bUsePointCloud = _bUsePointCloud;
    bUseMaskPixels = _bUseMaskPixels;
    bUseMaskTexture = _bUseMaskTexture;
    bUseDepthPixels = _bUseDepthPixels;
    bUseDepthTexture = _bUseDepthTexture;
    bNewPixels = false;
    bNewPointCloud = false;
}

void ofxOpenNIDepthThreshold::set(ofxOpenNIROI & _roi, 
                                  bool _bUsePointCloud,
                                  bool _bUseMaskPixels,
                                  bool _bUseMaskTexture,
                                  bool _bUseDepthPixels,
                                  bool _bUseDepthTexture,
                                  int _pointCloudDrawSize,
                                  int _pointCloudResolution){
    roi = _roi;
    ofPoint & lbn = roi.getLeftBottomNearWorld();
    ofPoint & rtf = roi.getRightTopFarWorld();
    if(lbn.x == 0 && lbn.y == 0 && rtf.x == 0 && rtf.y == 0){
        bUseXY = false;
    }else{
        bUseXY = true;
    }
    pointCloudDrawSize = _pointCloudDrawSize;
    pointCloudResolution = _pointCloudResolution;
    bUsePointCloud = _bUsePointCloud;
    bUseMaskPixels = _bUseMaskPixels;
    bUseMaskTexture = _bUseMaskTexture;
    bUseDepthPixels = _bUseDepthPixels;
    bUseDepthTexture = _bUseDepthTexture;
    bNewPixels = false;
    bNewPointCloud = false;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::drawPointCloud(){
    ofPushStyle();
    glPointSize(pointCloudDrawSize);
    glEnable(GL_DEPTH_TEST);
    pointCloud[1].drawVertices();
    glDisable(GL_DEPTH_TEST);
    ofPopStyle();
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::drawMask(){
    if (bUseMaskTexture){
        ofPushStyle();
        maskTexture.draw(0, 0, maskPixels.getWidth(), maskPixels.getHeight());
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::drawDepth(){
    if (bUseDepthTexture){
        ofPushStyle();
        depthTexture.draw(0, 0, depthPixels.getWidth(), depthPixels.getHeight());
        ofPopStyle();
    }
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::drawROI(){
    roi.drawROI();
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setNearThreshold(int _nearThreshold){
    CLAMP(_nearThreshold, 0, 10000);
    ofPoint & lbn = roi.getLeftBottomNearWorld();
    ofPoint & rtf = roi.getRightTopFarWorld();
    lbn.z = _nearThreshold;
    roi.set(lbn, rtf);
}

//--------------------------------------------------------------
int ofxOpenNIDepthThreshold::getNearThreshold(){
    return roi.getLeftBottomNearWorld().z;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setFarThreshold(int _farThreshold){
    CLAMP(_farThreshold, 0, 10000);
    ofPoint & lbn = roi.getLeftBottomNearWorld();
    ofPoint & rtf = roi.getRightTopFarWorld();
    rtf.z = _farThreshold;
    roi.set(lbn, rtf);
}

//--------------------------------------------------------------
int ofxOpenNIDepthThreshold::getFarThreshold(){
    return roi.getRightTopFarWorld().z;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setPointCloudDrawSize(int size){
    pointCloudDrawSize = size;
}

//--------------------------------------------------------------
int ofxOpenNIDepthThreshold::getPointCloudDrawSize(){
    return pointCloudDrawSize;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setPointCloudResolution(int resolution){
    pointCloudResolution = resolution;
}

//--------------------------------------------------------------
int ofxOpenNIDepthThreshold::getPointCloudResolution(){
    return pointCloudResolution;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setUseMaskTexture(bool b){
    bUseMaskTexture = b;
    if(bUseMaskTexture) bUseMaskPixels = true;
}

//--------------------------------------------------------------
bool ofxOpenNIDepthThreshold::getUseMaskTexture(){
    return bUseMaskTexture;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setUseMaskPixels(bool b){
    bUseMaskPixels = b;
}

//--------------------------------------------------------------
bool ofxOpenNIDepthThreshold::getUseMaskPixels(){
    return bUseMaskPixels;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setUseDepthTexture(bool b){
    bUseDepthTexture = b;
    if(bUseDepthTexture) bUseDepthPixels = true;
}

//--------------------------------------------------------------
bool ofxOpenNIDepthThreshold::getUseDepthTexture(){
    return bUseDepthTexture;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setUseDepthPixels(bool b){
    bUseDepthPixels = b;
}

//--------------------------------------------------------------
bool ofxOpenNIDepthThreshold::getUseDepthPixels(){
    return bUseDepthPixels;
}

//--------------------------------------------------------------
void ofxOpenNIDepthThreshold::setUsePointCloud(bool b){
    bUsePointCloud = b;
}

//--------------------------------------------------------------
bool ofxOpenNIDepthThreshold::getUsePointCloud(){
    return bUsePointCloud;
}

//--------------------------------------------------------------
ofMesh & ofxOpenNIDepthThreshold::getPointCloud(){
    return pointCloud[1];
}

//--------------------------------------------------------------
ofPixels & ofxOpenNIDepthThreshold::getMaskPixels(){
    return maskPixels;
}

//--------------------------------------------------------------
ofTexture & ofxOpenNIDepthThreshold::getMaskTextureReference(){
    return maskTexture;
}

//--------------------------------------------------------------
ofPixels & ofxOpenNIDepthThreshold::getDepthPixels(){
    return depthPixels;
}

//--------------------------------------------------------------
ofTexture & ofxOpenNIDepthThreshold::getDepthTextureReference(){
    return depthTexture;
}

//--------------------------------------------------------------
ofxOpenNIROI & ofxOpenNIDepthThreshold::getROI(){
    return roi;
}