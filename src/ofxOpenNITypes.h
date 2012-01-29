/*
 * ofxOpenNITypes.h
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

#ifndef	_H_OFXOPENNITYPES
#define _H_OFXOPENNITYPES

#include "ofxOpenNIUtils.h"

#include <XnTypes.h>
#include "ofConstants.h"
#include "ofPoint.h"
#include "ofMesh.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofGraphics.h"

class ofxOpenNIUserEvent {
    
public:
    
    ofxOpenNIUserEvent(){};
    ofxOpenNIUserEvent(XnUserID _userID, int _deviceID, UserStatusType _userStatus) 
    : userID(_userID), deviceID(_deviceID), userStatus(_userStatus){};
    
    XnUserID userID;
    UserStatusType userStatus;
    int deviceID;
    
};

class ofxOpenNIGestureEvent {
    
public:
    
    ofxOpenNIGestureEvent(){};
    ofxOpenNIGestureEvent(int _deviceID,
                          string _gestureName,
                          GestureStatusType _gestureStatus,
                          float _gestureProgress,
                          ofPoint _gesturePosition,
                          int _gestureTimestampMillis) 
    :
    deviceID(_deviceID),
    gestureName(_gestureName),
    gestureStatus(_gestureStatus),
    gestureProgress(_gestureProgress),
    gesturePosition(_gesturePosition),
    gestureTimestampMillis(_gestureTimestampMillis){};
    
    string gestureName;
    int deviceID;
    GestureStatusType gestureStatus;
    float gestureProgress;
    ofPoint gesturePosition;
    int gestureTimestampMillis;
    
};

class ofxOpenNIHandEvent {
    
public:
    
    ofxOpenNIHandEvent(){};
    ofxOpenNIHandEvent(XnUserID _handID,
                       int _deviceID,
                       string _handFocusGestureName,
                       HandStatusType _handStatus,
                       float _handFocusGestureProgress,
                       ofPoint _handFocusGesturePosition,
                       int _handTimestampMillis) 
    :
    handID(_handID),
    handFocusGestureName(_handFocusGestureName),
    deviceID(_deviceID),
    handStatus(_handStatus),
    handFocusGestureProgress(_handFocusGestureProgress),
    handFocusGesturePosition(_handFocusGesturePosition),
    handTimestampMillis(_handTimestampMillis){};
    
    XnUserID handID;
    int deviceID;
    string handFocusGestureName;
    HandStatusType handStatus;
    float handFocusGestureProgress;
    ofPoint handFocusGesturePosition;
    int handTimestampMillis;
    
};

class ofxOpenNIHand {
    
public:
    
    ofxOpenNIHand(){};
    
    XnUserID getID(){return id;};
    //bool isTracking(){return bIsTracking;};
    ofPoint & getPosition(){return position;};
    ofPoint & getWorldPosition(){return worldPosition;};
    
private:
    
    friend class ofxOpenNI;
    
    //bool bIsTracking;
	XnUserID id;
    ofPoint	position;
	ofPoint worldPosition;
    
};

class ofxOpenNILimb {

public:
    
	ofxOpenNILimb(XnSkeletonJoint nStartJoint, XnSkeletonJoint nEndJoint)
	:start_joint(nStartJoint), end_joint(nEndJoint),found(false){};

	ofxOpenNILimb(){};

	void set(XnSkeletonJoint nStartJoint, XnSkeletonJoint nEndJoint){
		start_joint = nStartJoint;
		end_joint = nEndJoint;
		found = false;
		begin.set(0,0,0);
		end.set(0,0,0);
	}

	XnSkeletonJoint start_joint;
	XnSkeletonJoint end_joint;
	XnSkeletonJointOrientation orientation;

	// position in projective coordinates
	ofPoint begin, end;

	// position in real world coordinates
	ofPoint worldBegin, worldEnd;
    
	bool found;

	void draw() {
		if(!found) return;
        ofPushStyle();
        if (start_joint == end_joint) {
            ofFill();
            ofCircle(begin.x, begin.y, 10);
        }else{
            ofSetLineWidth(5);
            ofLine(ofVec2f(begin),ofVec2f(end));
        }
		ofPopStyle();
	}
};

class ofxOpenNIUser {
    
public:
    
	ofxOpenNIUser();
	enum Limb {
        Head = 0,
		Neck,

        // hands
        LeftHand,
        RightHand,
        
		// left arm + shoulder
		LeftShoulder,
		LeftUpperArm,
		LeftLowerArm,

		// right arm + shoulder
		RightShoulder,
		RightUpperArm,
		RightLowerArm,

		// torso
		LeftUpperTorso,
		RightUpperTorso,

		// left lower torso + leg
		LeftLowerTorso,
		LeftUpperLeg,
		LeftLowerLeg,

		// right lower torso + leg
		RightLowerTorso,
		RightUpperLeg,
		RightLowerLeg,

		Hip,

		NumLimbs
	};

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
    
    void setLimbDetectionConfidence(float level);
    float getLimbDetectionConfidence();
    
    int getNumLimbs();
	ofxOpenNILimb & getLimb(Limb limb);
    
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
    
private:
    
    friend class ofxOpenNI; // so we can access directly in ofxOpenNI
    
	ofPoint center;
	vector<ofxOpenNILimb> limbs;
	ofMesh pointCloud[2];
	ofPixels maskPixels;
    ofTexture maskTexture;
    
    int id;
    
    int pointCloudDrawSize;
    int pointCloudResolution;
    
    float limbDetectionConfidence;
    
    bool bUseMaskPixels;
    bool bUseMaskTexture;
    bool bUsePointCloud;
    bool bUseAutoCalibration;
    bool bIsFound;
    bool bIsTracking;
    bool bIsSkeleton;
    bool bIsCalibrating;
    bool bNewPixels;
    bool bNewPointCloud;
    
    unsigned short* userPixels;

};

#endif
