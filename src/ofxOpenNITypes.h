/*
 * ofxOpenNITypes.h
 *
 * Copyright 2011 (c) Matthew Gingold [gameover] http://gingold.coau
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

#ifndef	_H_OFXOPENNITYPES
#define _H_OFXOPENNITYPES

#include "ofxOpenNIUtils.h"
#include "ofNode.h"

class ofxOpenNIJoint {
    
public:
    
    ofxOpenNIJoint(){
        type = JOINT_UNKOWN;
    };
    
	void set(XnSkeletonJoint _joint, float _confidenceThreshold = 0.5f){
		xnJoint = _joint;
        type = XnToOfJoint(xnJoint);
        bUseOrientation = false;
        bUseLocalOrientation = false;
        projectivePosition.set(0,0,0);
        worldPosition.set(0,0,0);
        originalOrientation.zeroRotation();
        mOrientation.zeroRotation();
        mDerivedOrientation.zeroRotation();
        positionConfidence = orientationConfidence = 0.0f;
        confidenceThreshold = _confidenceThreshold;
        parent = NULL;
        bUseOgreMethod = true;
	}

    void setParent(ofxOpenNIJoint & joint){
        this->parent = &joint;
    }
    
    ofxOpenNIJoint & getParent(){
        return (*parent);
    }
    
    string getName(){
        return getJointAsString(type);
    }
    
    Joint getType(){
        return type;
    }
    
    bool isParent(){
        return (parent != NULL);
    }
    
    void setInitialOrientation(ofQuaternion q){
        normalizeQuaternion(q);
        originalOrientation = q;
        mOrientation = q;
        mDerivedOrientation = q;
    }
    
    ofPoint & getProjectivePosition(){
        return projectivePosition;
    }
    
    ofPoint & getWorldPosition(){
        return worldPosition;
    }
    
    void calculateOrientation(){
        if(bUseOrientation){
            // THIS DOES NOT WORK YET!!!
            ofQuaternion newOrientation;
            newOrientation.zeroRotation();
            
            if(bUseOgreMethod){
                rotationMatrix.set(xnJointOrientation.orientation.elements[0],
                                   -xnJointOrientation.orientation.elements[1],
                                   xnJointOrientation.orientation.elements[2],
                                   -xnJointOrientation.orientation.elements[3],
                                   xnJointOrientation.orientation.elements[4],
                                   -xnJointOrientation.orientation.elements[5],
                                   xnJointOrientation.orientation.elements[6],
                                   -xnJointOrientation.orientation.elements[7],
                                   xnJointOrientation.orientation.elements[8]);
                
                rotationMatrixToQuaternian(rotationMatrix, newOrientation);
            }else{
                ofMatrix4x4 temp;
                temp(0,0) = xnJointOrientation.orientation.elements[0]; 
                temp(0,1) = -xnJointOrientation.orientation.elements[1]; 
                temp(0,2) = xnJointOrientation.orientation.elements[2]; 
                temp(0,3) = 0; 
                temp(1,0) = -xnJointOrientation.orientation.elements[3]; 
                temp(1,1) = xnJointOrientation.orientation.elements[4]; 
                temp(1,2) = -xnJointOrientation.orientation.elements[5]; 
                temp(1,3) = 0; 
                temp(2,0) = xnJointOrientation.orientation.elements[6]; 
                temp(2,1) = -xnJointOrientation.orientation.elements[7]; 
                temp(2,2) = xnJointOrientation.orientation.elements[8]; 
                temp(2,3) = 0; 
                temp(3,0) = 0; 
                temp(3,1) = 0; 
                temp(3,2) = 0; 
                temp(3,3) = 1; 
                newOrientation.set(temp);
            }
            
            //cout << "Transform " << getXNJointAsString(xnJoint);
            if(bUseLocalOrientation){
                newOrientation = convertWorldToLocalOrientation(newOrientation);
            }
            mOrientation.zeroRotation();
            mOrientation = newOrientation * originalOrientation;
            normalizeQuaternion(mOrientation);
            
            localEulerRotation = mOrientation.getEuler();
        }
    }
    
    ofQuaternion convertWorldToLocalOrientation(ofQuaternion & worldOrientation){
        //cout << " Start: ";
        updateFromParent();
        //cout << " :End" << endl;
        return mDerivedOrientation.inverse() * worldOrientation;
    }

    void updateFromParent(){
        //cout << "-> " << getXNJointAsString(xnJoint);
        if(isParent()){
            ofQuaternion & parentOrientation = parent->getDerivedOrientation();
            mDerivedOrientation = parentOrientation * mOrientation;
        }else{
            mDerivedOrientation = mOrientation;
        }
    }
    
    ofQuaternion & getDerivedOrientation(){
		updateFromParent();
        return mDerivedOrientation;
    }
    
    void setConfidenceThreshold(float _threshold){
        confidenceThreshold = _threshold;
    }
    
    float getConfidenceThreshold(){
        return confidenceThreshold;
    }
    
    float getPositionConfidence(){
        return positionConfidence;
    }
    
    float getOrientationConfidence(){
        return orientationConfidence;
    }
    
    void setUseOrientation(bool b){
        bUseOrientation = b;
    }
    
    bool getUseOrientation(){
        return bUseOrientation;
    }
    
    float getProjectiveDistanceToParent(){
        if(isParent()){
            return ofVec2f(parent->getProjectivePosition()).distance(ofVec2f(getProjectivePosition()));
        }else{
            return 0.0f;
        }
    }
    
	bool isFound(){
        return getPositionConfidence() > getConfidenceThreshold();// || getOrientationConfidence() > getConfidenceThreshold();
    }

	void draw(){
        ofPushStyle();

        if(bUseOrientation){
            ofPushMatrix();
            ofTranslate(projectivePosition.x, projectivePosition.y);//, projectivePosition.z);
            float angle; ofVec3f vec;
            mOrientation.getRotate(angle, vec);
            ofRotate(angle, vec.x, vec.y, vec.z);
            
            ofLine(0, 0, 0, getProjectiveDistanceToParent());
            
            ofDrawAxis(30);
            ofPopMatrix();
        }
        
        ofPushMatrix();
        
        if(getPositionConfidence() > 0.0f){
            ofTranslate(projectivePosition.x, projectivePosition.y);//, projectivePosition.z);
            if(isFound()){
                // draw in green
                ofSetColor(0, 255, 0);
            }else{
                // draw in red
                ofSetColor(255, 0, 0);
            }
            
            ofFill();
            ofCircle(0, 0, 5);
            ofNoFill();
        }
        
        if(isParent()){
            if(parent->getPositionConfidence() > 0.0f){
                if(isFound() && parent->isFound()){
                    // draw in green
                    ofSetColor(0, 255, 0);
                }else{
                    // draw in red
                    ofSetColor(255, 0, 0);
                }
                ofSetLineWidth(5);
                if(isParent()){
                    ofVec2f bone =  ofVec2f(parent->getProjectivePosition()) - ofVec2f(getProjectivePosition());
                    if(!bUseOrientation) ofLine(0, 0, 0, bone.x, bone.y, 0);
                }
            }
        }
        
        ofPopMatrix();
		ofPopStyle();
	}
    
    // experimental public switches
    bool bUseOgreMethod;
    bool bUseLocalOrientation;
    
protected:
    
    friend class ofxOpenNI;
    
    Joint type;
    
    // my parent joint
    ofxOpenNIJoint * parent;
    
    // xn joint and orientation
	XnSkeletonJoint xnJoint;
	XnSkeletonJointOrientation xnJointOrientation;
    
    // confidence levels & threshold
    float positionConfidence, orientationConfidence, confidenceThreshold;
    
	// position in projective coordinates
	ofPoint projectivePosition;
    
	// position in real world coordinates
	ofPoint worldPosition;

    // rotation matrix
    ofMatrix3x3 rotationMatrix;
    ofQuaternion originalOrientation;
    
    ofQuaternion mOrientation;
    ofQuaternion mDerivedOrientation;
    
    ofVec3f worldEulerRotation;
    ofVec3f localEulerRotation;

    bool bUseOrientation;

};

class ofxOpenNILimb {

public:
    
	ofxOpenNILimb(){};

	void set(ofxOpenNIJoint & _startJoint, ofxOpenNIJoint & _endJoint){
		this->startJoint = &_startJoint;
		this->endJoint = &_endJoint;
	}

    ofxOpenNIJoint & getStartJoint(){
        return (*startJoint);
    }
    
    ofxOpenNIJoint & getEndJoint(){
        return (*endJoint);
    }
    
    bool isFound(){
        return startJoint->isFound() && endJoint->isFound();
    }
    
	void draw() {

        ofPushStyle();
        if(isFound()){
            // draw in green
            ofSetColor(0, 255, 0);
        }else{
            // draw in red
            ofSetColor(255, 0, 0);
        }
        ofSetLineWidth(5);
        ofLine(ofVec2f(startJoint->getProjectivePosition()),ofVec2f(endJoint->getProjectivePosition()));
		ofPopStyle();
	}

private:
    
    ofxOpenNIJoint * startJoint;
	ofxOpenNIJoint * endJoint;
    
};

class ofxOpenNIUser {
    
public:
    
	ofxOpenNIUser();
    ~ofxOpenNIUser();
    
    void setup();
    void update();
    void clear();
    
    void drawSkeleton();
    void drawPointCloud();
    void drawMask();
    
    void setUseAutoCalibration(bool b);
    bool getUseAutoCalibration();
    
    void setPointCloudDrawSize(int size); // this is the size of the points when drawing
    int getPointCloudDrawSize();
    
    void setPointCloudResolution(int resolution); // this is the step size when calculating (lower is higher res!)
    int getPointCloudResolution();
    
    void setUseMaskTexture(bool b);
    bool getUseMaskTexture();
    
    void setUseMaskPixels(bool b);
    bool getUseMaskPixels();
    
    void setUsePointCloud(bool b);
    bool getUsePointCloud();
    
    void setUseSkeleton(bool b);
    bool getUseSkeleton();
    
    void setUseOrientation(bool b);
    bool getUseOrientation();
    
    void setConfidenceThreshold(float confidenceThreshold);
    float getConfidenceThreshold();
    
    void setForceResetTimeout(int millis);
    int getForceResetTimeout();
    
    void setForceReset(bool useTimeout = true, bool forceImmediateRestart = false);
    bool getForceReset();
    
    int getNumLimbs();
	ofxOpenNILimb & getLimb(Limb limb);
    
    int getNumJoints();
	ofxOpenNIJoint & getJoint(Joint joint);
    
    ofPoint & getCenter();
    ofMesh & getPointCloud();
    ofPixels & getMaskPixels();
    ofTexture & getMaskTextureReference();
    
    int getID();
    
    bool isFound();
    bool isTracking();
    bool isSkeleton();
    bool isCalibrating();

    string getDebugInfo();
    
    ofxOpenNIUser(const ofxOpenNIUser& other){
        if(this != &other){
            
            clear();
            setup();
            
            pointCloudDrawSize = other.pointCloudDrawSize;
            pointCloudResolution = other.pointCloudResolution;
            confidenceThreshold = other.confidenceThreshold;
            forcedResetTimeout = other.forcedResetTimeout;
            resetCount = other.resetCount;
            
            bUseMaskPixels = other.bUseMaskPixels;
            bUseMaskTexture = other.bUseMaskTexture;
            bUseSkeleton = other.bUseSkeleton;
            bUsePointCloud = other.bUsePointCloud;
            bUseAutoCalibration = other.bUseAutoCalibration;
            bForceReset = other.bForceReset;
            bForceRestart = other.bForceRestart;
            bUseOrientation = other.bUseOrientation;
            
            id = other.id;
            
            //cout << "COPY$$$$$$$$$$$$$$$$$ " << bUseSkeleton << endl;
        }
        
        return *this;
    }
    
private:
    
    friend class ofxOpenNI; // so we can access directly in ofxOpenNI
    
	ofPoint center;
    vector<ofxOpenNIJoint> joints;
	vector<ofxOpenNILimb> limbs;
	ofMesh pointCloud[2];
	ofPixels maskPixels;
    ofTexture maskTexture;
    
    int id;
    
    int pointCloudDrawSize;
    int pointCloudResolution;
    
    float confidenceThreshold;
    int resetCount, forcedResetTimeout;
    
    bool bUseMaskPixels;
    bool bUseMaskTexture;
    bool bUseSkeleton;
    bool bUsePointCloud;
    bool bUseAutoCalibration;
    bool bIsFound;
    bool bIsTracking;
    bool bIsSkeleton;
    bool bIsCalibrating;
    bool bNewPixels;
    bool bNewPointCloud;
    bool bForceReset;
    bool bForceRestart;
    bool bUseOrientation;
    
    unsigned short * userPixels;
    
    ofxOpenNIUser& operator=(const ofxOpenNIUser& other){
        if(this != &other){

            clear();
            setup();
            
            pointCloudDrawSize = other.pointCloudDrawSize;
            pointCloudResolution = other.pointCloudResolution;
            confidenceThreshold = other.confidenceThreshold;
            forcedResetTimeout = other.forcedResetTimeout;
            resetCount = other.resetCount;
            
            bUseMaskPixels = other.bUseMaskPixels;
            bUseMaskTexture = other.bUseMaskTexture;
            bUseSkeleton = other.bUseSkeleton;
            bUsePointCloud = other.bUsePointCloud;
            bUseAutoCalibration = other.bUseAutoCalibration;
            bForceReset = other.bForceReset;
            bForceRestart = other.bForceRestart;
            bUseOrientation = other.bUseOrientation;
            
            id = other.id;
            
            //cout << "ASGN$$$$$$$$$$$$$$$$$ " << bUseSkeleton << endl;
        }
        
        return *this;
    }
};

class ofxOpenNIUserEvent {
    
public:
    
    ofxOpenNIUserEvent(int _deviceID, UserStatusType _userStatus, XnUserID _id, int _timestampMillis) 
    : deviceID(_deviceID), userStatus(_userStatus), id(_id), timestampMillis(_timestampMillis){};
    
    int deviceID;
    UserStatusType userStatus;
    XnUserID id;
    int timestampMillis;
    
    //ofxOpenNIUser * user;
    
};

class ofxOpenNIDepthThreshold {
    
public:
    
    ofxOpenNIDepthThreshold();
    ofxOpenNIDepthThreshold(int _nearThreshold,
                            int _farThreshold,
                            bool _bUsePointCloud = false,
                            bool _bUseMaskPixels = true,
                            bool _bUseMaskTexture = true,
                            bool _bUseDepthPixels = false,
                            bool _bUseDepthTexture = false)
    :
    nearThreshold(_nearThreshold),
    farThreshold(_farThreshold),
    bUsePointCloud(_bUsePointCloud),
    bUseMaskPixels(_bUseMaskPixels || _bUseMaskTexture),
    bUseMaskTexture(_bUseMaskTexture),
    bUseDepthPixels(_bUseDepthPixels || _bUseDepthTexture),
    bUseDepthTexture(_bUseDepthTexture){
        pointCloudDrawSize = 2;
        pointCloudResolution = 2;
        bNewPixels = false;
        bNewPointCloud = false;};
    
    void drawPointCloud();
    void drawMask();
    void drawDepth();
    
    void setNearThreshold(int _nearThreshold);
    int getNearThreshold();
    
    void setFarThreshold(int _farThreshold);
    int getFarThreshold();
    
    void setPointCloudDrawSize(int size); // this is the size of the points when drawing
    int getPointCloudDrawSize();
    
    void setPointCloudResolution(int resolution); // this is the step size when calculating (lower is higher res!)
    int getPointCloudResolution();
    
    void setUseMaskTexture(bool b);
    bool getUseMaskTexture();
    
    void setUseMaskPixels(bool b);
    bool getUseMaskPixels();
    
    void setUseDepthTexture(bool b);
    bool getUseDepthTexture();
    
    void setUseDepthPixels(bool b);
    bool getUseDepthPixels();
    
    void setUsePointCloud(bool b);
    bool getUsePointCloud();
    
    ofMesh & getPointCloud();
    ofPixels & getMaskPixels();
    ofTexture & getMaskTextureReference();
    ofPixels & getDepthPixels();
    ofTexture & getDepthTextureReference();
    
private:
    
    friend class ofxOpenNI;
    
    int nearThreshold;
    int farThreshold;
    
	ofMesh pointCloud[2];
    ofPixels depthPixels;
    ofTexture depthTexture;
	ofPixels maskPixels;
    ofTexture maskTexture;
    
    int id;
    
    int pointCloudDrawSize;
    int pointCloudResolution;
    
    bool bUseDepthPixels;
    bool bUseDepthTexture;
    bool bUseMaskPixels;
    bool bUseMaskTexture;
    bool bUsePointCloud;
    bool bNewPixels;
    bool bNewPointCloud;
    
};

class ofxOpenNIHotspot {
    
public:
    
    ofxOpenNIHotspot(){};
    
    ofxOpenNIHotspot(ofPoint _leftBottomNearWorld, ofPoint _rightTopFarWorld, xn::DepthGenerator & g_Depth){
        set(_leftBottomNearWorld, _rightTopFarWorld, g_Depth);
    };
    
    void set(ofPoint _leftBottomNearWorld, ofPoint _rightTopFarWorld, xn::DepthGenerator & g_Depth){
        leftBottomNearWorld = _leftBottomNearWorld;
        rightTopFarWorld = _rightTopFarWorld;
        if(&g_Depth != NULL){
            
            leftBottomNearProjective = worldToProjective(leftBottomNearWorld, g_Depth);
            ofPoint rightTopNearWorld = ofPoint(rightTopFarWorld.x, rightTopFarWorld.y, leftBottomNearWorld.z);
            rightTopNearProjective = worldToProjective(rightTopNearWorld, g_Depth);
            
            rightTopFarProjective = worldToProjective(rightTopFarWorld, g_Depth);
            ofPoint leftBottomFarWorld = ofPoint(leftBottomNearWorld.x, leftBottomNearWorld.y, rightTopFarWorld.z);
            leftBottomFarProjective = worldToProjective(leftBottomFarWorld, g_Depth);
            
            nearPlane.set(ofVec2f(leftBottomNearProjective), rightTopNearProjective.x - leftBottomNearProjective.x, rightTopNearProjective.y - leftBottomNearProjective.y);
            farPlane.set(ofVec2f(leftBottomFarProjective), rightTopFarProjective.x - leftBottomFarProjective.x, rightTopFarProjective.y - leftBottomFarProjective.y);
            
            centerWorld = leftBottomNearWorld.middle(rightTopFarWorld);
            centerProjective = worldToProjective(centerWorld, g_Depth);
            maxDistance = centerWorld.distance(leftBottomNearWorld);
        }
    }
    
    bool drawInside(ofxOpenNIUser & user){
        for(int j = 0; j < user.getNumJoints(); j++){
            ofxOpenNIJoint & joint = user.getJoint((Joint)j);
            return drawInside(joint);
        }
    }
    
    bool drawInside(ofxOpenNIJoint & joint){
        if(inside(joint)){
            ofPushStyle();
            float magnitude = distanceToCenter(joint)/maxDistance;
            ofSetColor(255*magnitude, 0, 255);
            ofCircle(ofVec2f(joint.getProjectivePosition()), 5);
            ofSetLineWidth(2);
            ofLine(ofVec2f(joint.getProjectivePosition()), ofVec2f(centerProjective));
            ofPopStyle();
            return true;
        }
        return false;
    }
    
    void draw(){
        ofPushStyle();
        
        ofNoFill();
        ofSetLineWidth(0.5);
        ofCircle(ofVec2f(centerProjective), 5);
        
        ofRect(nearPlane);
        ofRect(farPlane);
        
        ofLine(ofVec2f(leftBottomNearProjective), ofVec2f(leftBottomFarProjective));
        ofLine(ofVec2f(rightTopNearProjective), ofVec2f(rightTopFarProjective));
        ofLine(ofVec2f(leftBottomNearProjective.x, rightTopNearProjective.y), ofVec2f(leftBottomFarProjective.x, rightTopFarProjective.y));
        ofLine(ofVec2f(rightTopNearProjective.x, leftBottomNearProjective.y), ofVec2f(rightTopFarProjective.x, leftBottomFarProjective.y));

        ofPopStyle();
    }
    
    inline float distanceToCenter(ofxOpenNIJoint & joint){
        return centerWorld.distance(joint.getProjectivePosition());
    }
    
    inline int numJointsInside(ofxOpenNIUser & user){
        int limbsInside = 0;
        for(int j = 0; j < user.getNumJoints(); j++){
            ofxOpenNIJoint & joint = user.getJoint((Joint)j);
            if(inside(joint)) limbsInside++;
        }
        return limbsInside;
    }
    
    inline float percentInside(ofxOpenNIUser & user){
        return (float)numJointsInside(user)/user.getNumJoints();
    }
    
    inline bool inside(ofxOpenNIUser & user){
        inside(user, user.getNumJoints());
    }
    
    inline bool inside(ofxOpenNIUser & user, float pct){
        int numLimbs = floor(user.getNumJoints() * pct);
        inside(user, numLimbs);
    }
    
    inline bool inside(ofxOpenNIUser & user, int numLimbs){
        if(numLimbs > user.getNumJoints()){
            ofLogError() << "More limbs than the user for inside test!!";
            return false;
        }
        int limbsInside = -numLimbs;
        for(int j = 0; j < user.getNumJoints(); j++){
            ofxOpenNIJoint & joint = user.getJoint((Joint)j);
            if(inside(joint)) limbsInside++;
            if(limbsInside == numLimbs){
                return true;
            }
        }
        return false;
    }
    
    inline bool inside(ofxOpenNILimb & limb){
        return inside(limb.getStartJoint().getWorldPosition()) && inside(limb.getEndJoint().getWorldPosition());
    }
    
    inline bool inside(ofxOpenNIJoint & joint){
        return inside(joint.getWorldPosition());
    }
    
    inline bool inside(ofPoint & worldPosition){
        if(worldPosition.x > leftBottomNearWorld.x && worldPosition.y > leftBottomNearWorld.y && worldPosition.y > leftBottomNearWorld.y &&
           worldPosition.x < rightTopFarWorld.x && worldPosition.y < rightTopFarWorld.y && worldPosition.y < rightTopFarWorld.y){
            //ofLogVerbose() << "Inside hotspot";
            return true;
        }else{
            return false;
        }
    }
    
    float maxDistance;
    ofPoint centerWorld, centerProjective;
    
    ofPoint leftBottomNearWorld, rightTopFarWorld;
    ofRectangle nearPlane, farPlane;
    ofPoint leftBottomNearProjective, rightTopFarProjective;
    ofPoint rightTopNearProjective, leftBottomFarProjective;
};


class ofxOpenNIGestureEvent {
    
public:
    
    ofxOpenNIGestureEvent(){};
    ofxOpenNIGestureEvent(int _deviceID,
                          string _gestureName,
                          GestureStatusType _gestureStatus,
                          float _progress,
                          ofPoint _position,
                          ofPoint _worldPosition,
                          int _timestampMillis)
    :
    deviceID(_deviceID),
    gestureName(_gestureName),
    gestureStatus(_gestureStatus),
    progress(_progress),
    position(_position),
    worldPosition(_worldPosition),
    timestampMillis(_timestampMillis){};
    
    int deviceID;
    string gestureName;
    GestureStatusType gestureStatus;
    float progress;
    ofPoint position;
    ofPoint worldPosition;
    int timestampMillis;
    
};

class ofxOpenNIHand {
    
public:
    
    ofxOpenNIHand(){};
    
    XnUserID getID(){return id;};
    ofPoint & getPosition(){return position;};
    ofPoint & getWorldPosition(){return worldPosition;};
    
    bool isTracking(){return bIsTracking;};
    
private:
    
    friend class ofxOpenNI;
    
	XnUserID id;
    ofPoint	position;
	ofPoint worldPosition;
    
    bool bIsTracking;
    
    //    // block copy ctor and assignment operator
    //    ofxOpenNIHand(const ofxOpenNIHand& other);
    //    ofxOpenNIHand& operator=(const ofxOpenNIHand&);
    
};

class ofxOpenNIHandEvent {
    
public:
    
    ofxOpenNIHandEvent(){};
    ofxOpenNIHandEvent(int _deviceID, HandStatusType _handStatus, XnUserID _id, ofPoint _position, ofPoint _worldPosition, int _timestampMillis)
    : deviceID(_deviceID), handStatus(_handStatus), id(_id), position(_position), worldPosition(_worldPosition), timestampMillis(_timestampMillis){};
    
    int deviceID;
    HandStatusType handStatus;
    XnUserID id;
    ofPoint position;
    ofPoint worldPosition;
    int timestampMillis;
    
    //ofxOpenNIHand * hand;
    
};

#endif
