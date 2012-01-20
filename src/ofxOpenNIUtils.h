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

#include <XnTypes.h>
#include "ofPoint.h"
#include "ofTypes.h"

#define SHOW_RC(rc, what)                                                   \
ofLogVerbose(LOG_NAME) << what << "status:" << xnGetStatusString(rc);
#define BOOL_RC(rc, what)                                                   \
if (rc != XN_STATUS_OK) {                                                   \
ofLogError(LOG_NAME) << what << "status:" << xnGetStatusString(rc);         \
return false;                                                               \
}else{                                                                      \
ofLogVerbose(LOG_NAME) << what << "status:" << xnGetStatusString(rc);       \
return true;                                                                \
}
#define CHECK_ERR_RC(rc, what)                                              \
if (rc != XN_STATUS_OK) ofLogError(LOG_NAME) << what << "status:" << xnGetStatusString(rc);
#define BOOL_ERR_RC(rc, what)                                               \
if (rc != XN_STATUS_OK) {                                                   \
ofLogError(LOG_NAME) << what << "status:" << xnGetStatusString(rc);         \
return false;                                                               \
}

static bool rainbowPalletInit = false;

static XnUInt8 PalletIntsR [256] = {0};
static XnUInt8 PalletIntsG [256] = {0};
static XnUInt8 PalletIntsB [256] = {0};

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

class ofxOpenNIUserEvent {
    
public:
    
    ofxOpenNIUserEvent(){};
    ofxOpenNIUserEvent(XnUserID _userID, int _deviceID, UserStatusType _userStatus) : userID(_userID), deviceID(_deviceID), userStatus(_userStatus){};
    
    XnUserID userID;
    UserStatusType userStatus;
    int deviceID;
    
};

class ofxOpenNIGestureEvent {
    
public:
    
    ofxOpenNIGestureEvent(){};
    ofxOpenNIGestureEvent(string _gestureName, int _deviceID, GestureStatusType _gestureStatus, float _gestureProgress, ofPoint _gesturePosition, int _gestureTimestampMillis) : gestureName(_gestureName), deviceID(_deviceID), gestureStatus(_gestureStatus), gestureProgress(_gestureProgress), gesturePosition(_gesturePosition), gestureTimestampMillis(_gestureTimestampMillis){};
    
    string gestureName;
    GestureStatusType gestureStatus;
    float gestureProgress;
    ofPoint gesturePosition;
    int gestureTimestampMillis;
    int deviceID;
    
};

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
    
    switch (depthColoring){
        case COLORING_PSYCHEDELIC_SHADES:
            color.a *= (((XnFloat)(depth % 10) / 20) + 0.5);
        case COLORING_PSYCHEDELIC:
            switch ((depth/10) % 10){
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

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize);

//class ofxOpenNICube{
//    
//public:
//    
//    ofxOpenNICube();
//    
//    ofPoint LeftBottomNear;
//    ofPoint RightTopFar;
//    
//};
//
////--------------------------------------------------------------
//inline ofxOpenNICube toOf(const XnBoundingBox3D & r){
//    return *(ofxOpenNICube*)&r;
//}

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

//--------------------------------------------------------------
inline string boolToString(bool b){
    return (string)(b ? "TRUE" : "FALSE");
}

//--------------------------------------------------------------
inline string getGestureStatusAsString(GestureStatusType type) {
	switch (type) {
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
inline string getUserStatusAsString(UserStatusType type) {
	switch (type) {
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
inline string getCalibrationStatusAsString(XnCalibrationStatus type) {
	switch (type) {
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
inline string getNodeTypeAsString(XnProductionNodeType type) {
	switch (type) {
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
		default:
			return "UNKNOWN_NODE_TYPE";
			break;
	}
}

#endif