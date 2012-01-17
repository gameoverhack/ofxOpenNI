#pragma once

#include <XnTypes.h>
#include "ofConstants.h"
#include "ofPoint.h"
#include "ofMesh.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofGraphics.h"

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

	vector<ofxOpenNILimb*> jointLimbs;

	void draw() {
		if(!found) return;
		//ofPushStyle();
		ofSetLineWidth(5);
		//ofSetColor(255,0,0);
		ofLine(ofVec2f(begin),ofVec2f(end));
		//ofPopStyle();
	}
};

class ofxOpenNIUser {
    
public:
    
	ofxOpenNIUser();

	enum Limb{
		Neck = 0,

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
    
    void setCloudPointSize(int size); // this is the size of the points when drawing
    int getCloudPointSize();
    
    void setCloudPointResolution(int resolution); // this is the step size when calculating (lower is higher res!)
    int getCloudPointResolution();
    
    void setUseTexture(bool b);
    
    void setUseMask(bool b);
    bool getUseMask();
    
    void setUsePointCloud(bool b);
    bool getUsePointCloud();
    
    int getNumLimbs();
	ofxOpenNILimb & getLimb(Limb limb);
    
    ofPoint & getCenter();
    ofMesh & getPointCloud();
    ofPixels & getMaskPixels();
    ofTexture & getMaskTextureReference();
    
    int getID();
    
    bool isTracking();
    
private:
    
    friend class ofxOpenNI; // so we can access directly in ofxOpenNI
    
	ofPoint center;
	vector<ofxOpenNILimb> limbs;
	ofMesh pointCloud;
	ofPixels maskPixels;
    ofTexture maskTexture;
    
    int id;
    int cloudPointSize;
    int cloudPointResolution;
    
    bool bUseMask;
    bool bUsePointCloud;
    bool bUseTexture;
    bool bIsFound;
    bool bIsTracking;
    bool bIsCalibrating;
    bool bIsCalibrated;
    bool bIsAllocated;

};
