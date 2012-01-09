/*
 * ofxGestureGenerator.cpp
 *
 * Copyright 2011 (c) Matthew Gingold http://gingold.com.au
 * Originally forked from a project by roxlu http://www.roxlu.com/ 
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

#include "ofxGestureGenerator.h"
#include "ofxOpenNIMacros.h"

// ctor
//--------------------------------------------------------------
ofxGestureGenerator::ofxGestureGenerator() {
	// default to not using progress events
	b_use_progress = false;
	min_time = 0;
}

// dtor
//--------------------------------------------------------------
ofxGestureGenerator::~ofxGestureGenerator() {
	// TODO: Unregister callbacks on shutdown
	gesture_generator.Unref();
}

// GESTURE CALLBACKS
// =============================================================================
// Callback: New Gesture was detected
void XN_CALLBACK_TYPE
OpenNI_Gesture_Recognized(xn::GestureGenerator& generator,
                   const XnChar* strGesture,
                   const XnPoint3D* pIDPosition,
                   const XnPoint3D* pEndPosition, void* pCookie) 
{	
	
	ofxGestureGenerator* ofx_gesture_generator = static_cast<ofxGestureGenerator*>(pCookie);

	ofLog(OF_LOG_VERBOSE, "Gesture RECOGNIZED: %s  posID [%d, %d, %d]  posEND [%d, %d, %d]\n", strGesture,
							(int)pIDPosition->X, (int)pIDPosition->Y, (int)pIDPosition->Z,
							(int)pEndPosition->X, (int)pEndPosition->Y, (int)pEndPosition->Z);	
	
	gesture * last_gesture = ofx_gesture_generator->getLastGesture();
	
	if (last_gesture->gesture_timestamp == 0 || ofGetElapsedTimeMillis() > last_gesture->gesture_timestamp + ofx_gesture_generator->getMinTimeBetweenGestures()) { // Filter by a minimum time between firing gestures
		
		ofLog(OF_LOG_VERBOSE, "Gesture EVENT triggered!\n");
		
		last_gesture->gesture_name = strGesture;
		last_gesture->gesture_type = GESTURE_RECOGNIZED;
		last_gesture->gesture_position.x = pEndPosition->X;	
		last_gesture->gesture_position.y = pEndPosition->Y;	
		last_gesture->gesture_position.z = pEndPosition->Z;	
		last_gesture->gesture_progress = 1.0;
		last_gesture->gesture_timestamp = ofGetElapsedTimeMillis();
		
		ofNotifyEvent(ofx_gesture_generator->gestureRecognized, *last_gesture);
	} else {
		ofLog(OF_LOG_VERBOSE, "Gesture FILTERED by time: %d < %d\n", ofGetElapsedTimeMillis(), last_gesture->gesture_timestamp + ofx_gesture_generator->getMinTimeBetweenGestures());
	}

}

// Callback: Gesture processing
void XN_CALLBACK_TYPE
OpenNI_Gesture_Process(xn::GestureGenerator& generator,
                const XnChar* strGesture,
                const XnPoint3D* pIDPosition,
                XnFloat fProgress,
                void* pCookie)
{
	
	ofxGestureGenerator* ofx_gesture_generator = static_cast<ofxGestureGenerator*>(pCookie);
	
	if (ofx_gesture_generator->getUseProgress()) {
		
		ofLog(OF_LOG_VERBOSE, "Gesture RECOGNIZED: %s  posID [%d, %d, %d]  progress [%f]\n", strGesture,
								(int)pIDPosition->X, (int)pIDPosition->Y, (int)pIDPosition->Z, (float)fProgress);
		
		gesture * last_gesture = ofx_gesture_generator->getLastGesture();
		
		last_gesture->gesture_name = strGesture;
		last_gesture->gesture_type = GESTURE_PROGRESS;
		last_gesture->gesture_position.x = pIDPosition->X;	
		last_gesture->gesture_position.y = pIDPosition->Y;	
		last_gesture->gesture_position.z = pIDPosition->Z;	
		last_gesture->gesture_progress = (float)fProgress;
		last_gesture->gesture_timestamp = ofGetElapsedTimeMillis();
		
		ofNotifyEvent(ofx_gesture_generator->gestureRecognized, *last_gesture);
	}
}

//--------------------------------------------------------------
bool ofxGestureGenerator::setup(ofxOpenNIContext* pContext) {
	
	pContext->getDepthGenerator(&depth_generator);
	
	XnStatus result = XN_STATUS_OK;	
	XnMapOutputMode map_mode;
	
	// Try to fetch gesture generator before creating one
	if(pContext->getGestureGenerator(&gesture_generator)) {
		// found the gesture generator - for do nothing		
	} else {
		result = gesture_generator.Create(pContext->getXnContext());
		CHECK_RC(result, "Creating gesture generator");
		
		if (result != XN_STATUS_OK) return false;
		
	}

	last_gesture.gesture_timestamp = 0;
	
	XnCallbackHandle gesture_cb_handle;
	gesture_generator.RegisterGestureCallbacks(OpenNI_Gesture_Recognized, OpenNI_Gesture_Process, this, gesture_cb_handle);
	
	gesture_generator.StartGenerating();	
	
	printf("Gesture generator inited\n");
	
	return true;
	
}

//--------------------------------------------------------------
bool ofxGestureGenerator::addGesture(string gesture_name) {
	XnStatus result = XN_STATUS_OK;	
	
	result = gesture_generator.AddGesture(gesture_name.c_str(), NULL);
	string msg = "Adding simple (openNI) gesture " + gesture_name;
	BOOL_RC(result, msg.c_str());
}

//--------------------------------------------------------------
bool ofxGestureGenerator::removeGesture(string gesture_name) {
	XnStatus result = XN_STATUS_OK;	
	
	result = gesture_generator.RemoveGesture(gesture_name.c_str());
	string msg = "Removing simple (openNI) gesture " + gesture_name;
	BOOL_RC(result, msg.c_str());
}

//--------------------------------------------------------------
void ofxGestureGenerator::setUseProgress(bool b) {
	b_use_progress = b;
}

//--------------------------------------------------------------
bool ofxGestureGenerator::getUseProgress() {
	return b_use_progress;
}

//--------------------------------------------------------------
void ofxGestureGenerator::setMinTimeBetweenGestures(int time) {
	if (time > 0) min_time = time; // do we want a max?? no...
}

//--------------------------------------------------------------
int ofxGestureGenerator::getMinTimeBetweenGestures() {
	return min_time;
}

//--------------------------------------------------------------
gesture* ofxGestureGenerator::getLastGesture() {
	return &last_gesture;
}

//--------------------------------------------------------------
xn::GestureGenerator& ofxGestureGenerator::getXnGestureGenerator() {
	return gesture_generator;
}