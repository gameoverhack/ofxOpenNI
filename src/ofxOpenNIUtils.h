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

#define SHOW_RC(rc, what)                                                   \
ofLogVerbose(LOG_NAME) << what << "status:" << xnGetStatusString(rc);
#define BOOL_RC(rc, what)                                                   \
ofLogVerbose(LOG_NAME) << what << "status:" << xnGetStatusString(rc);       \
return (rc == XN_STATUS_OK);
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

void YUV422ToRGB888(const XnUInt8* pYUVImage, XnUInt8* pRGBImage, XnUInt32 nYUVSize, XnUInt32 nRGBSize);

inline ofPoint toOf(const XnPoint3D & p){
	return *(ofPoint*)&p;
	/*
	 this is more future safe, but currently unnecessary and slower:
	 return ofPoint(p.X,p.Y,p.Z);
	 */
}

inline XnPoint3D toXn(const ofPoint & p){
	
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