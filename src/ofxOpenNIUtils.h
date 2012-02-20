/*
 * ofxOpenNIUtils.h
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

#ifndef _H_OFXOPENNIUTILS
#define _H_OFXOPENNIUTILS

#include <set>
#include <map>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <XnTypes.h>

#include "ofConstants.h"
#include "ofPoint.h"
#include "ofMesh.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofGraphics.h"
#include "ofMatrix3x3.h"
#include "ofQuaternion.h"
#include "of3dUtils.h"
#include "ofLog.h"
#include "ofThread.h"
#include "ofEvents.h"
#include "ofNode.h"
#include "ofCamera.h"

#define YUV422_U  0
#define YUV422_Y1 1
#define YUV422_V  2
#define YUV422_Y2 3
#define YUV422_BPP 4
#define YUV_RED   0
#define YUV_GREEN 1
#define YUV_BLUE  2
#define YUV_ALPHA  3
#define YUV_RGBA_BPP 4

#ifdef TARGET_WIN32
#ifdef __INTEL_COMPILER
#include <ia32intrin.h>
#else
#include <intrin.h>
#endif
#endif

#define SHOW_RC(rc, what)                                                   \
ofLogVerbose(LOG_NAME) << what << "status:" << xnGetStatusString(rc);
#define BOOL_RC(rc, what)                                                   \
if (rc != XN_STATUS_OK){                                                   \
ofLogError(LOG_NAME) << what << "status:" << xnGetStatusString(rc);         \
return false;                                                               \
}else{                                                                      \
ofLogVerbose(LOG_NAME) << what << "status:" << xnGetStatusString(rc);       \
return true;                                                                \
}
#define CHECK_ERR_RC(rc, what)                                              \
if (rc != XN_STATUS_OK) ofLogError(LOG_NAME) << what << "status:" << xnGetStatusString(rc);
#define BOOL_ERR_RC(rc, what)                                               \
if (rc != XN_STATUS_OK){                                                   \
ofLogError(LOG_NAME) << what << "status:" << xnGetStatusString(rc);         \
return false;                                                               \
}

static bool rainbowPalletInit = false;

static XnUInt8 PalletIntsR [256] = {0};
static XnUInt8 PalletIntsG [256] = {0};
static XnUInt8 PalletIntsB [256] = {0};

enum ONITask {
    ONI_START_RECORD = 0,
    ONI_STOP_RECORD,
    ONI_NONE
};

enum DepthColoring {
    COLORING_PSYCHEDELIC_SHADES = 0,
    COLORING_PSYCHEDELIC,
    COLORING_RAINBOW,
    COLORING_CYCLIC_RAINBOW,
    COLORING_BLUES,
    COLORING_GREY,
    COLORING_STATUS,
    COLORING_COUNT
};

enum UserStatusType {
    USER_TRACKING_STOPPED = 0,
    USER_TRACKING_STARTED,
    USER_CALIBRATION_STOPPED,
    USER_CALIBRATION_STARTED,
    USER_SKELETON_LOST,
    USER_SKELETON_FOUND
};

enum GestureStatusType {
    GESTURE_PROGRESS = 0,
    GESTURE_RECOGNIZED
};

enum HandStatusType {
    HAND_TRACKING_STOPPED = 0,
    HAND_TRACKING_STARTED,
    HAND_TRACKING_UPDATED,
    HAND_FOCUS_GESTURE_PROGRESS,
    HAND_FOCUS_GESTURE_RECOGNIZED,
};

enum Joint {
    
    // start at root joint
    JOINT_TORSO = 0,
    JOINT_NECK,
    JOINT_HEAD,
    
    // left arm + shoulder
    JOINT_LEFT_SHOULDER,
    JOINT_LEFT_ELBOW,
    JOINT_LEFT_HAND,
    
    // right arm + shoulder
    JOINT_RIGHT_SHOULDER,
    JOINT_RIGHT_ELBOW,
    JOINT_RIGHT_HAND,
    
    // left leg
    JOINT_LEFT_HIP,
    JOINT_LEFT_KNEE,
    JOINT_LEFT_FOOT,
    
    // right leg
    JOINT_RIGHT_HIP,
    JOINT_RIGHT_KNEE,
    JOINT_RIGHT_FOOT,
    
    JOINT_COUNT,
    JOINT_UNKOWN
};

enum Limb {
    
    // left upper torso + arm
    LIMB_LEFT_UPPER_TORSO = 0,
    LIMB_LEFT_SHOULDER,
    LIMB_LEFT_UPPER_ARM,
    LIMB_LEFT_LOWER_ARM,
    
    // left lower torso + leg
    LIMB_LEFT_LOWER_TORSO,
    LIMB_LEFT_UPPER_LEG,
    LIMB_LEFT_LOWER_LEG,
    
    // right upper torso + arm
    LIMB_RIGHT_UPPER_TORSO,
    LIMB_RIGHT_SHOULDER,
    LIMB_RIGHT_UPPER_ARM,
    LIMB_RIGHT_LOWER_ARM,
    
    // right lower torso + leg
    LIMB_RIGHT_LOWER_TORSO,
    LIMB_RIGHT_UPPER_LEG,
    LIMB_RIGHT_LOWER_LEG,
    
    LIMB_NECK,
    LIMB_PELVIS,
    
    LIMB_COUNT,
    LIMB_UNKOWN
};

//-----------------------------------------------------------------------
static inline void rotationMatrixToQuaternian(ofMatrix3x3 & kRot, ofQuaternion & q){
    // Converted from Ogre's Quaternion::FromRotationMatrix (const Matrix3& kRot)
    
    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation".
    
    // Ogre matrix to of matrix elements
    //[0][0]a [0][1]b [0][2]c
    //[1][0]d [1][1]e [1][2]f
    //[2][0]g [2][1]h [2][2]i
    //kRot.transpose(); // not necessary?
    
    float fTrace = kRot[0 * 3 + 0] + kRot[1 * 3 + 1] + kRot[2 * 3 + 2];
    float fRoot;
    
    if(fTrace > 0.0){
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0f);  // 2w
        q.w() = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;  // 1/(4w)
        q.x() = (kRot[2 * 3 + 1] - kRot[1 * 3 + 2]) * fRoot;
        q.y() = (kRot[0 * 3 + 2] - kRot[2 * 3 + 0]) * fRoot;
        q.z() = (kRot[1 * 3 + 0] - kRot[0 * 3 + 1]) * fRoot;
    }else{
        // |w| <= 1/2
        static size_t s_iNext[3] = { 1, 2, 0 };
        size_t i = 0;
        if ( kRot[1 * 3 + 1] > kRot[0 * 3 + 0] )
            i = 1;
        if ( kRot[2 * 3 + 2] > kRot[i * 3 + i] )
            i = 2;
        size_t j = s_iNext[i];
        size_t k = s_iNext[j];
        
        fRoot = sqrt(kRot[i * 3 + i] - kRot[j * 3 + j] - kRot[k * 3 + k] + 1.0f);
        float* apkQuat[3] = { &q.x(), &q.y(), &q.z() };
        *apkQuat[i] = 0.5f * fRoot;
        fRoot = 0.5f / fRoot;
        q.w() = (kRot[k * 3 + j] - kRot[j * 3 + k]) * fRoot;
        *apkQuat[j] = (kRot[j * 3 + i] + kRot[i * 3 + j]) * fRoot;
        *apkQuat[k] = (kRot[k * 3 + i] + kRot[i * 3 + k]) * fRoot;
        
    }
};

//-----------------------------------------------------------------------
static inline void quaternianToRotationMatrix(ofQuaternion & q, ofMatrix3x3 & kRot){
    float fTx  = q.x() + q.x();
    float fTy  = q.x() + q.x();
    float fTz  = q.z() + q.z();
    float fTwx = fTx * q.w();
    float fTwy = fTy * q.w();
    float fTwz = fTz * q.w();
    float fTxx = fTx * q.x();
    float fTxy = fTy * q.x();
    float fTxz = fTz * q.x();
    float fTyy = fTy * q.y();
    float fTyz = fTz * q.y();
    float fTzz = fTz * q.z();
    
    kRot[0 * 3 + 0] = 1.0f-(fTyy+fTzz);
    kRot[0 * 3 + 1] = fTxy-fTwz;
    kRot[0 * 3 + 2] = fTxz+fTwy;
    kRot[1 * 3 + 0] = fTxy+fTwz;
    kRot[1 * 3 + 1] = 1.0f-(fTxx+fTzz);
    kRot[1 * 3 + 2] = fTyz-fTwx;
    kRot[2 * 3 + 0] = fTxz-fTwy;
    kRot[2 * 3 + 1] = fTyz+fTwx;
    kRot[2 * 3 + 2] = 1.0f-(fTxx+fTyy);
};

//-----------------------------------------------------------------------
static inline void quaternianFromAngleAxis(ofQuaternion & q, float rfAngle, ofVec3f rkAxis)
{
    // assert:  axis[] is unit length
    //
    // The quaternion representing the rotation is
    //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
    
    float fHalfAngle(0.5 * rfAngle);
    float fSin = sin(fHalfAngle);
    q.w() = cos(fHalfAngle);
    q.x() = fSin * rkAxis.x;
    q.y() = fSin * rkAxis.y;
    q.z() = fSin * rkAxis.z;
}

//-----------------------------------------------------------------------
static inline void quaternionToAxes(ofQuaternion & q, ofVec3f & xaxis, ofVec3f & yaxis, ofVec3f & zaxis){
    ofMatrix3x3 kRot;
    
    quaternianToRotationMatrix(q, kRot);
    
    xaxis.x = kRot[0 * 3 + 0];
    xaxis.y = kRot[1 * 3 + 0];
    xaxis.z = kRot[2 * 3 + 0];
    
    yaxis.x = kRot[0 * 3 + 1];
    yaxis.y = kRot[1 * 3 + 1];
    yaxis.z = kRot[2 * 3 + 1];
    
    zaxis.x = kRot[0 * 3 + 2];
    zaxis.y = kRot[1 * 3 + 2];
    zaxis.z = kRot[2 * 3 + 2];
};

static inline void normalizeQuaternion(ofQuaternion & q){
    float len = q.w()*q.w()+q.x()*q.x()+q.y()*q.y()+q.z()*q.z();
    float factor = 1.0f / sqrt(len);
    q = q * factor;
}

//--------------------------------------------------------------
static void CreateRainbowPallet(){
	if (rainbowPalletInit) return;
	
	unsigned char r, g, b;
	for (int i=1; i<255; i++){
		if (i<=29){
			r = (unsigned char)(129.36-i*4.36);
			g = 0;
			b = (unsigned char)255;
		}
		else if (i<=86){
			r = 0;
			g = (unsigned char)(-133.54+i*4.52);
			b = (unsigned char)255;
		}
		else if (i<=141){
			r = 0;
			g = (unsigned char)255;
			b = (unsigned char)(665.83-i*4.72);
		}
		else if (i<=199){
			r = (unsigned char)(-635.26+i*4.47);
			g = (unsigned char)255;
			b = 0;
		}
		else {
			r = (unsigned char)255;
			g = (unsigned char)(1166.81-i*4.57);
			b = 0;
		}
		PalletIntsR[i] = r;
		PalletIntsG[i] = g;
		PalletIntsB[i] = b;
	}
	rainbowPalletInit = true;
}

//--------------------------------------------------------------
inline void getDepthColor(DepthColoring depthColoring, const unsigned short & depth, ofColor & color, int maxDepth){
    
    float max;
    XnUInt16 col_index;
    
    switch(depthColoring){
        case COLORING_PSYCHEDELIC_SHADES:
            color.a *= (((XnFloat)(depth % 10) / 20) + 0.5);
        case COLORING_PSYCHEDELIC:
            switch((depth/10) % 10){
                case 0:
                    color.r = 255;
                    break;
                case 1:
                    color.g = 255;
                    break;
                case 2:
                    color.b = 255;
                    break;
                case 3:
                    color.r = 255;
                    color.g = 255;
                    break;
                case 4:
                    color.g = 255;
                    color.b = 255;
                    break;
                case 5:
                    color.r = 255;
                    color.b = 255;
                    break;
                case 6:
                    color.r = 255;
                    color.g = 255;
                    color.b = 255;
                    break;
                case 7:
                    color.r = 127;
                    color.b = 255;
                    break;
                case 8:
                    color.r = 255;
                    color.b = 127;
                    break;
                case 9:
                    color.r = 127;
                    color.g = 255;
                    break;
            }
            break;
        case COLORING_RAINBOW:
            col_index = (XnUInt16)(((depth) / (maxDepth / 256)));
            color.r = PalletIntsR[col_index];
            color.g = PalletIntsG[col_index];
            color.b = PalletIntsB[col_index];
            break;
        case COLORING_CYCLIC_RAINBOW:
            col_index = (depth % 256);
            color.r = PalletIntsR[col_index];
            color.g = PalletIntsG[col_index];
            color.b = PalletIntsB[col_index];
            break;
        case COLORING_BLUES:
            // 3 bytes of depth: black (R0G0B0) >> color.b (001) >> cyan (011) >> white (111)
            max = 256+255+255;
            col_index = (XnUInt16)(((depth) / (maxDepth / max)));
            if ( col_index < 256 )
            {
                color.b	= col_index;
                color.g	= 0;
                color.r	= 0;
            }
            else if ( col_index < (256+255) )
            {
                color.b	= 255;
                color.g	= (col_index % 256) + 1;
                color.r	= 0;
            }
            else if ( col_index < (256+255+255) )
            {
                color.b	= 255;
                color.g	= 255;
                color.r	= (col_index % 256) + 1;
            }
            else
            {
                color.b	= 255;
                color.g	= 255;
                color.r	= 255;
            }
            break;
        case COLORING_GREY:
            max = 255;	// half depth
        {
            XnUInt8 a = (XnUInt8)(((depth) / (maxDepth / max)));
            color.r	= a;
            color.g	= a;
            color.b	= a;
        }
            break;
        case COLORING_STATUS:
            // This is something to use on installations
            // when the end user needs to know if the camera is tracking or not
            // The scene will be painted color.g if status == true
            // The scene will be painted color.r if status == false
            // Usage: declare a global bool status and that's it!
            // I'll keep it commented so you dont have to have a status on every project
#if 0
        {
            extern bool status;
            max = 255;	// half depth
            XnUInt8 a = 255 - (XnUInt8)(((depth) / (maxDepth / max)));
            color.r	= ( status ? 0 : a);
            color.g	= ( status ? a : 0);
            color.b	= 0;
        }
#endif
            break;
    }
}

//--------------------------------------------------------------
static inline ofPoint toOf(const XnPoint3D & p){
	return *(ofPoint*)&p;
	/*
	 this is more future safe, but currently unnecessary and slower:
	 return ofPoint(p.X,p.Y,p.Z);
	 */
}

//--------------------------------------------------------------
static inline XnPoint3D toXn(const ofPoint & p){
	return *(XnPoint3D*)&p;
	/*
	 this is more future safe, but currently unnecessary and slower:
	 XnPoint3D r;
	 r.X = p.x;
	 r.Y = p.y;
	 r.Z = p.z;
	 return r;
	 */
}

#define WIDTH 640
#define HEIGHT 480
#define MAXDEPTH 10000
#define HALFWIDTH WIDTH/2
#define HALFHEIGHT HEIGHT/2
#define HFOV 1.01447
#define VFOV 0.789809
#define COEFX WIDTH/HFOV
#define COEFY HEIGHT/VFOV
#define HFOVTANHALF tan(HFOV/2)
#define VFOVTANHALF tan(VFOV/2)

//--------------------------------------------------------------
static inline ofPoint g_worldToProjective(const ofPoint& p){
    // this assumes a 640 x 480 resolution as per above defines
    ofPoint projective;
	projective.x = COEFX * p.x / p.z + HALFWIDTH;
    projective.y = HALFHEIGHT - COEFY * p.y / p.z;
    projective.z = p.z;
	return projective;
}

//--------------------------------------------------------------
static inline ofPoint g_worldToProjective(const XnVector3D& p){
	return g_worldToProjective(toOf(p));
}

//--------------------------------------------------------------
static inline ofPoint worldToProjective(const XnVector3D& p, xn::DepthGenerator & g_Depth){
    //if(&g_Depth == NULL) return g_worldToProjective(p);
	XnVector3D proj;
	g_Depth.ConvertRealWorldToProjective(1,&p,&proj);
	return toOf(proj);
}

//--------------------------------------------------------------
static inline ofPoint worldToProjective(const ofPoint& p, xn::DepthGenerator & g_Depth){
    //if(&g_Depth == NULL) return g_worldToProjective(p);
	XnVector3D world = toXn(p);
	return worldToProjective(world, g_Depth);
}

//--------------------------------------------------------------
static inline void g_maxROIAtDepth(ofPoint& leftBottomNear, ofPoint& rightTopFar){
    // this assumes a 640 x 480 resolution as per above defines
    leftBottomNear.x = -(leftBottomNear.z * HFOVTANHALF);
    leftBottomNear.y = -(leftBottomNear.z * VFOVTANHALF);
    rightTopFar.x = (rightTopFar.z * HFOVTANHALF);
    rightTopFar.y = (rightTopFar.z * VFOVTANHALF);
}

//--------------------------------------------------------------
static inline void maxROIAtDepth(ofPoint& leftBottomNear, ofPoint& rightTopFar, xn::DepthGenerator & g_Depth){
    //if(&g_Depth == NULL) return g_maxROIAtDepth(leftBottomNear, rightTopFar);
    XnFieldOfView FOV;
    g_Depth.GetFieldOfView(FOV);
    leftBottomNear.x = -(leftBottomNear.z * tan(FOV.fHFOV/2));
    leftBottomNear.y = -(leftBottomNear.z * tan(FOV.fVFOV/2));
    rightTopFar.x = (rightTopFar.z * tan(FOV.fHFOV/2));
    rightTopFar.y = (rightTopFar.z * tan(FOV.fVFOV/2));
}

//--------------------------------------------------------------
inline string boolToString(bool b){
    return (string)(b ? "TRUE" : "FALSE");
}

inline Joint XnToOfJoint(XnSkeletonJoint type){
    switch(type){
        case XN_SKEL_HEAD:
			return JOINT_HEAD;
			break;
		case XN_SKEL_NECK:
			return JOINT_NECK;
			break;
        case XN_SKEL_LEFT_HAND:
			return JOINT_LEFT_HAND;
			break;
        case XN_SKEL_RIGHT_HAND:
			return JOINT_RIGHT_HAND;
			break;
		case XN_SKEL_LEFT_SHOULDER:
			return JOINT_LEFT_SHOULDER;
			break;
        case XN_SKEL_RIGHT_SHOULDER:
			return JOINT_RIGHT_SHOULDER;
			break;
        case XN_SKEL_RIGHT_ELBOW:
			return JOINT_RIGHT_ELBOW;
			break;
        case XN_SKEL_LEFT_ELBOW:
			return JOINT_LEFT_ELBOW;
            break;
//        case XN_SKEL_LEFT_COLLAR:
//			return "XN_SKEL_LEFT_COLLAR";
//			break;
//        case XN_SKEL_LEFT_WRIST:
//			return "XN_SKEL_LEFT_WRIST";
//			break;
//		case XN_SKEL_LEFT_FINGERTIP:
//			return "XN_SKEL_LEFT_FINGERTIP";
//			break;
//        case XN_SKEL_RIGHT_COLLAR:
//			return "XN_SKEL_RIGHT_COLLAR";
//			break;
//        case XN_SKEL_RIGHT_WRIST:
//			return "XN_SKEL_RIGHT_WRIST";
//			break;
//        case XN_SKEL_RIGHT_FINGERTIP:
//			return "XN_SKEL_RIGHT_FINGERTIP";
//			break;
        case XN_SKEL_TORSO:
			return JOINT_TORSO;
			break;
//        case XN_SKEL_WAIST:
//			return "XN_SKEL_WAIST";
//			break;
        case XN_SKEL_LEFT_HIP:
			return JOINT_LEFT_HIP;
			break;
        case XN_SKEL_LEFT_KNEE:
			return JOINT_LEFT_KNEE;
			break;
        case XN_SKEL_LEFT_FOOT:
			return JOINT_LEFT_FOOT;
			break;
        case XN_SKEL_RIGHT_HIP:
			return JOINT_RIGHT_FOOT;
			break;
        case XN_SKEL_RIGHT_KNEE:
			return JOINT_RIGHT_KNEE;
			break;
        case XN_SKEL_RIGHT_FOOT:
			return JOINT_RIGHT_FOOT;
			break;
//        case XN_SKEL_LEFT_ANKLE:
//			return "XN_SKEL_LEFT_ANKLE";
//			break;
//        case XN_SKEL_RIGHT_ANKLE:
//			return "XN_SKEL_RIGHT_ANKLE";
//			break;
		default:
			return JOINT_UNKOWN;
			break;
    }
}

//--------------------------------------------------------------
inline string getXNJointAsString(XnSkeletonJoint type){
	switch(type){
        case XN_SKEL_HEAD:
			return "XN_SKEL_HEAD";
			break;
		case XN_SKEL_NECK:
			return "XN_SKEL_NECK";
			break;
        case XN_SKEL_LEFT_HAND:
			return "XN_SKEL_LEFT_HAND";
			break;
        case XN_SKEL_RIGHT_HAND:
			return "XN_SKEL_RIGHT_HAND";
			break;
		case XN_SKEL_LEFT_SHOULDER:
			return "XN_SKEL_LEFT_SHOULDER";
			break;
        case XN_SKEL_RIGHT_SHOULDER:
			return "XN_SKEL_RIGHT_SHOULDER";
			break;
        case XN_SKEL_RIGHT_ELBOW:
			return "XN_SKEL_RIGHT_ELBOW";
			break;
        case XN_SKEL_LEFT_ELBOW:
			return "XN_SKEL_LEFT_ELBOW";
            break;
        case XN_SKEL_LEFT_COLLAR:
			return "XN_SKEL_LEFT_COLLAR";
			break;
        case XN_SKEL_LEFT_WRIST:
			return "XN_SKEL_LEFT_WRIST";
			break;
		case XN_SKEL_LEFT_FINGERTIP:
			return "XN_SKEL_LEFT_FINGERTIP";
			break;
        case XN_SKEL_RIGHT_COLLAR:
			return "XN_SKEL_RIGHT_COLLAR";
			break;
        case XN_SKEL_RIGHT_WRIST:
			return "XN_SKEL_RIGHT_WRIST";
			break;
        case XN_SKEL_RIGHT_FINGERTIP:
			return "XN_SKEL_RIGHT_FINGERTIP";
			break;
        case XN_SKEL_TORSO:
			return "XN_SKEL_TORSO";
			break;
        case XN_SKEL_WAIST:
			return "XN_SKEL_WAIST";
			break;
        case XN_SKEL_LEFT_HIP:
			return "XN_SKEL_LEFT_HIP";
			break;
        case XN_SKEL_LEFT_KNEE:
			return "XN_SKEL_LEFT_KNEE";
			break;
        case XN_SKEL_LEFT_FOOT:
			return "XN_SKEL_LEFT_FOOT";
			break;
        case XN_SKEL_RIGHT_HIP:
			return "XN_SKEL_RIGHT_HIP";
			break;
        case XN_SKEL_RIGHT_KNEE:
			return "XN_SKEL_RIGHT_KNEE";
			break;
        case XN_SKEL_RIGHT_FOOT:
			return "XN_SKEL_RIGHT_FOOT";
			break;
        case XN_SKEL_LEFT_ANKLE:
			return "XN_SKEL_LEFT_ANKLE";
			break;
        case XN_SKEL_RIGHT_ANKLE:
			return "XN_SKEL_RIGHT_ANKLE";
			break;
		default:
			return "UNKNOWN_XN_SKEL_TYPE";
			break;
    }
}

//--------------------------------------------------------------
inline string getJointAsString(Joint type){
	switch(type){
        case JOINT_HEAD:
			return "JOINT_HEAD";
			break;
		case JOINT_NECK:
			return "JOINT_NECK";
			break;
        case JOINT_LEFT_HAND:
			return "JOINT_LEFT_HAND";
			break;
        case JOINT_RIGHT_HAND:
			return "JOINT_RIGHT_HAND";
			break;
		case JOINT_LEFT_SHOULDER:
			return "JOINT_LEFT_SHOULDER";
			break;
        case JOINT_RIGHT_SHOULDER:
			return "JOINT_RIGHT_SHOULDER";
			break;
        case JOINT_RIGHT_ELBOW:
			return "JOINT_RIGHT_ELBOW";
			break;
        case JOINT_LEFT_ELBOW:
			return "JOINT_LEFT_ELBOW";
			break;
        case JOINT_TORSO:
			return "JOINT_TORSO";
			break;
        case JOINT_LEFT_HIP:
			return "JOINT_LEFT_HIP";
			break;
        case JOINT_LEFT_KNEE:
			return "JOINT_LEFT_KNEE";
			break;
        case JOINT_LEFT_FOOT:
			return "JOINT_LEFT_FOOT";
			break;
        case JOINT_RIGHT_HIP:
			return "JOINT_RIGHT_HIP";
			break;
        case JOINT_RIGHT_KNEE:
			return "JOINT_RIGHT_KNEE";
			break;
        case JOINT_RIGHT_FOOT:
			return "JOINT_RIGHT_FOOT";
			break;
		default:
			return "JOINT_UNKOWN";
			break;
    }
}

//--------------------------------------------------------------
inline string getLimbAsString(Limb type){
	switch(type){
        case LIMB_NECK:
			return "LIMB_NECK";
			break;
		case LIMB_LEFT_SHOULDER:
			return "LIMB_LEFT_SHOULDER";
			break;
        case LIMB_LEFT_UPPER_ARM:
			return "LIMB_LEFT_UPPER_ARM";
			break;
        case LIMB_LEFT_LOWER_ARM:
			return "LIMB_LEFT_LOWER_ARM";
			break;
		case LIMB_RIGHT_SHOULDER:
			return "LIMB_RIGHT_SHOULDER";
			break;
        case LIMB_RIGHT_UPPER_ARM:
			return "LIMB_RIGHT_UPPER_ARM";
			break;
        case LIMB_RIGHT_LOWER_ARM:
			return "LIMB_RIGHT_LOWER_ARM";
			break;
        case LIMB_LEFT_UPPER_TORSO:
			return "LIMB_LEFT_UPPER_TORSO";
			break;
        case LIMB_RIGHT_UPPER_TORSO:
			return "LIMB_RIGHT_UPPER_TORSO";
			break;
        case LIMB_LEFT_LOWER_TORSO:
			return "LIMB_LEFT_LOWER_TORSO";
			break;
        case LIMB_LEFT_UPPER_LEG:
			return "LIMB_LEFT_UPPER_LEG";
			break;
        case LIMB_LEFT_LOWER_LEG:
			return "LIMB_LEFT_LOWER_LEG";
			break;
        case LIMB_RIGHT_LOWER_TORSO:
			return "LIMB_RIGHT_LOWER_TORSO";
			break;
        case LIMB_RIGHT_UPPER_LEG:
			return "LIMB_RIGHT_UPPER_LEG";
			break;
        case LIMB_RIGHT_LOWER_LEG:
			return "LIMB_RIGHT_LOWER_LEG";
			break;
        case LIMB_PELVIS:
			return "LIMB_PELVIS";
			break;
		default:
			return "LIMB_UNKOWN";
			break;
    }
}

//--------------------------------------------------------------
inline string getHandStatusAsString(HandStatusType type){
	switch(type){
        case HAND_TRACKING_STOPPED:
			return "HAND_TRACKING_STOPPED";
			break;
		case HAND_TRACKING_STARTED:
			return "HAND_TRACKING_STARTED";
			break;
        case HAND_TRACKING_UPDATED:
			return "HAND_TRACKING_UPDATED";
			break;
        case HAND_FOCUS_GESTURE_PROGRESS:
			return "HAND_FOCUS_GESTURE_PROGRESS";
			break;
		case HAND_FOCUS_GESTURE_RECOGNIZED:
			return "HAND_FOCUS_GESTURE_RECOGNIZED";
			break;
		default:
			return "UNKNOWN_HAND_STATUS_TYPE";
			break;
    }
}

//--------------------------------------------------------------
inline string getGestureStatusAsString(GestureStatusType type){
	switch(type){
        case GESTURE_PROGRESS:
			return "GESTURE_PROGRESS";
			break;
		case GESTURE_RECOGNIZED:
			return "GESTURE_RECOGNIZED";
			break;
		default:
			return "UNKNOWN_GESTURE_STATUS_TYPE";
			break;
    }
}

//--------------------------------------------------------------
inline string getUserStatusAsString(UserStatusType type){
	switch(type){
        case USER_TRACKING_STOPPED:
			return "USER_TRACKING_STOPPED";
			break;
		case USER_TRACKING_STARTED:
			return "USER_TRACKING_STARTED";
			break;
		case USER_CALIBRATION_STOPPED:
			return "USER_CALIBRATION_STOPPED";
			break;
		case USER_CALIBRATION_STARTED:
			return "USER_CALIBRATION_STARTED";
			break;
		case USER_SKELETON_LOST:
			return "USER_SKELETON_LOST";
			break;
		case USER_SKELETON_FOUND:
			return "USER_SKELETON_FOUND";
			break;
		default:
			return "UNKNOWN_USER_STATUS_TYPE";
			break;
    }
}

//--------------------------------------------------------------
inline string getCalibrationStatusAsString(XnCalibrationStatus type){
	switch(type){
		case XN_CALIBRATION_STATUS_OK:
			return "XN_CALIBRATION_STATUS_OK";
			break;
		case XN_CALIBRATION_STATUS_NO_USER:
			return "XN_CALIBRATION_STATUS_NO_USER";
			break;
		case XN_CALIBRATION_STATUS_ARM:
			return "XN_CALIBRATION_STATUS_ARM";
			break;
		case XN_CALIBRATION_STATUS_LEG:
			return "XN_CALIBRATION_STATUS_LEG";
			break;
		case XN_CALIBRATION_STATUS_HEAD:
			return "XN_CALIBRATION_STATUS_HEAD";
			break;
		case XN_CALIBRATION_STATUS_TORSO:
			return "XN_CALIBRATION_STATUS_TORSO";
			break;
		case XN_CALIBRATION_STATUS_TOP_FOV:
			return "XN_CALIBRATION_STATUS_TOP_FOV";
			break;
		case XN_CALIBRATION_STATUS_SIDE_FOV:
			return "XN_CALIBRATION_STATUS_SIDE_FOV";
			break;
		case XN_CALIBRATION_STATUS_POSE:
			return "XN_CALIBRATION_STATUS_POSE";
			break;
		case XN_CALIBRATION_STATUS_MANUAL_ABORT:
			return "XN_CALIBRATION_STATUS_MANUAL_ABORT";
			break;
		case XN_CALIBRATION_STATUS_MANUAL_RESET:
			return "XN_CALIBRATION_STATUS_MANUAL_RESET";
			break;
		case XN_CALIBRATION_STATUS_TIMEOUT_FAIL:
			return "XN_CALIBRATION_STATUS_TIMEOUT_FAIL";
			break;
		default:
			return "UNKNOWN_CALIBRATION_STATUS_TYPE";
			break;
	}
}

//--------------------------------------------------------------
inline string getNodeTypeAsString(XnProductionNodeType type){
	switch(type){
		case XN_NODE_TYPE_INVALID:
			return "XN_NODE_TYPE_INVALID";
			break;
		case XN_NODE_TYPE_DEVICE:
			return "XN_NODE_TYPE_DEVICE";
			break;
		case XN_NODE_TYPE_DEPTH:
			return "XN_NODE_TYPE_DEPTH";
			break;
		case XN_NODE_TYPE_IMAGE:
			return "XN_NODE_TYPE_IMAGE";
			break;
		case XN_NODE_TYPE_AUDIO:
			return "XN_NODE_TYPE_AUDIO";
			break;
		case XN_NODE_TYPE_IR:
			return "XN_NODE_TYPE_IR";
			break;
		case XN_NODE_TYPE_USER:
			return "XN_NODE_TYPE_USER";
			break;
		case XN_NODE_TYPE_RECORDER:
			return "XN_NODE_TYPE_RECORDER";
			break;
		case XN_NODE_TYPE_GESTURE:
			return "XN_NODE_TYPE_GESTURE";
			break;
		case XN_NODE_TYPE_SCENE:
			return "XN_NODE_TYPE_SCENE";
			break;
		case XN_NODE_TYPE_HANDS:
			return "XN_NODE_TYPE_HANDS";
			break;
		case XN_NODE_TYPE_CODEC:
			return "XN_NODE_TYPE_CODEC";
			break;
		case XN_NODE_TYPE_PRODUCTION_NODE:
			return "XN_NODE_TYPE_PRODUCTION_NODE";
			break;
		case XN_NODE_TYPE_GENERATOR:
			return "XN_NODE_TYPE_GENERATOR";
			break;
		case XN_NODE_TYPE_MAP_GENERATOR:
			return "XN_NODE_TYPE_MAP_GENERATOR";
			break;
		case XN_NODE_TYPE_SCRIPT:
			return "XN_NODE_TYPE_SCRIPT";
			break;
		case XN_NODE_TYPE_FIRST_EXTENSION:
			return "XN_NODE_TYPE_FIRST_EXTENSION";
			break;
        case XN_NODE_TYPE_PLAYER:
            return "XN_NODE_TYPE_PLAYER";
		default:
			return "UNKNOWN_NODE_TYPE";
			break;
	}
}

#ifdef TARGET_WIN32

//--------------------------------------------------------------
inline void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBAImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize){
	const XnUInt8* pYUVLast = pYUVImage + nYUVSize - 8;
	XnUInt8* pRGBLast = pRGBAImage + nRGBSize - 16;
	
	const __m128 minus16 = _mm_set_ps1(-16);
	const __m128 minus128 = _mm_set_ps1(-128);
	const __m128 plus113983 = _mm_set_ps1(1.13983F);
	const __m128 minus039466 = _mm_set_ps1(-0.39466F);
	const __m128 minus058060 = _mm_set_ps1(-0.58060F);
	const __m128 plus203211 = _mm_set_ps1(2.03211F);
	const __m128 zero = _mm_set_ps1(0);
	const __m128 plus255 = _mm_set_ps1(255);
	
	// define YUV floats
	__m128 y;
	__m128 u;
	__m128 v;
	
	__m128 temp;
	
	// define RGB floats
	__m128 r;
	__m128 g;
	__m128 b;
	
	// define RGB integers
	__m128i iR;
	__m128i iG;
	__m128i iB;
	
	XnUInt32* piR = (XnUInt32*)&iR;
	XnUInt32* piG = (XnUInt32*)&iG;
	XnUInt32* piB = (XnUInt32*)&iB;
	
	while (pYUVImage <= pYUVLast && pRGBAImage <= pRGBLast)
	{
		// process 4 pixels at once (values should be ordered backwards)
		y = _mm_set_ps(pYUVImage[YUV422_Y2 + YUV422_BPP], pYUVImage[YUV422_Y1 + YUV422_BPP], pYUVImage[YUV422_Y2], pYUVImage[YUV422_Y1]);
		u = _mm_set_ps(pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U + YUV422_BPP],  pYUVImage[YUV422_U],  pYUVImage[YUV422_U]);
		v = _mm_set_ps(pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V + YUV422_BPP],  pYUVImage[YUV422_V],  pYUVImage[YUV422_V]);
		
		u = _mm_add_ps(u, minus128); // u -= 128
		v = _mm_add_ps(v, minus128); // v -= 128
		
		/*
		 
		 http://en.wikipedia.org/wiki/YUV
		 
		 From YUV to RGB:
		 R =     Y + 1.13983 V
		 G =     Y - 0.39466 U - 0.58060 V
		 B =     Y + 2.03211 U
		 
		 */
		
		temp = _mm_mul_ps(plus113983, v);
		r = _mm_add_ps(y, temp);
		
		temp = _mm_mul_ps(minus039466, u);
		g = _mm_add_ps(y, temp);
		temp = _mm_mul_ps(minus058060, v);
		g = _mm_add_ps(g, temp);
		
		temp = _mm_mul_ps(plus203211, u);
		b = _mm_add_ps(y, temp);
		
		// make sure no value is smaller than 0
		r = _mm_max_ps(r, zero);
		g = _mm_max_ps(g, zero);
		b = _mm_max_ps(b, zero);
		
		// make sure no value is bigger than 255
		r = _mm_min_ps(r, plus255);
		g = _mm_min_ps(g, plus255);
		b = _mm_min_ps(b, plus255);
		
		// convert floats to int16 (there is no conversion to uint8, just to int8).
		iR = _mm_cvtps_epi32(r);
		iG = _mm_cvtps_epi32(g);
		iB = _mm_cvtps_epi32(b);
		
		// extract the 4 pixels RGB values.
		// because we made sure values are between 0 and 255, we can just take the lower byte
		// of each INT16
		pRGBAImage[0] = piR[0];
		pRGBAImage[1] = piG[0];
		pRGBAImage[2] = piB[0];
		pRGBAImage[3] = 255;
		
		pRGBAImage[4] = piR[1];
		pRGBAImage[5] = piG[1];
		pRGBAImage[6] = piB[1];
		pRGBAImage[7] = 255;
		
		pRGBAImage[8] = piR[2];
		pRGBAImage[9] = piG[2];
		pRGBAImage[10] = piB[2];
		pRGBAImage[11] = 255;
		
		pRGBAImage[12] = piR[3];
		pRGBAImage[13] = piG[3];
		pRGBAImage[14] = piB[3];
		pRGBAImage[15] = 255;
		
		// advance the streams
		pYUVImage += 8;
		pRGBAImage += 16;
	}
}

#else // not Win32

//--------------------------------------------------------------
inline void YUV444ToRGBA(XnUInt8 cY, XnUInt8 cU, XnUInt8 cV, XnUInt8& cR, XnUInt8& cG, XnUInt8& cB, XnUInt8& cA){
	XnInt32 nC = cY - 16;
	XnInt16 nD = cU - 128;
	XnInt16 nE = cV - 128;
	
	nC = nC * 298 + 128;
	
	cR = MIN(MAX((nC            + 409 * nE) >> 8, 0), 255);
	cG = MIN(MAX((nC - 100 * nD - 208 * nE) >> 8, 0), 255);
	cB = MIN(MAX((nC + 516 * nD           ) >> 8, 0), 255);
	cA = 255;
}

//--------------------------------------------------------------
inline void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize){
	const XnUInt8* pCurrYUV = pYUVImage;
	XnUInt8* pCurrRGB = pRGBImage;
	const XnUInt8* pLastYUV = pYUVImage + nYUVSize - YUV422_BPP;
	XnUInt8* pLastRGB = pRGBImage + nRGBSize - YUV_RGBA_BPP;
	
	while (pCurrYUV <= pLastYUV && pCurrRGB <= pLastRGB)
	{
		YUV444ToRGBA(pCurrYUV[YUV422_Y1], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
					 pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE], pCurrRGB[YUV_ALPHA]);
		pCurrRGB += YUV_RGBA_BPP;
		YUV444ToRGBA(pCurrYUV[YUV422_Y2], pCurrYUV[YUV422_U], pCurrYUV[YUV422_V],
					 pCurrRGB[YUV_RED], pCurrRGB[YUV_GREEN], pCurrRGB[YUV_BLUE], pCurrRGB[YUV_ALPHA]);
		pCurrRGB += YUV_RGBA_BPP;
		pCurrYUV += YUV422_BPP;
	}
}

#endif

#endif