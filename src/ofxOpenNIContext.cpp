/*
 *  ofxOpenNIContext.cpp
 *  opennisample
 *
 *  Created by Jonas Jongejan on 06/01/11.
 *
 */

#include "ofxOpenNIContext.h"



ofxOpenNIContext::ofxOpenNIContext(){
}

bool ofxOpenNIContext::setup(){
	//Init OpenNI context
	XnStatus nRetVal = g_Context.Init();
	
	if (nRetVal != XN_STATUS_OK){
		printf("Setup OpenNI context failed: %s\n", xnGetStatusString(nRetVal));
		return false;
	} else {
		ofLog(OF_LOG_VERBOSE, "OpenNI Context inited");
		return true;
	}
}

void ofxOpenNIContext::update(){
	XnStatus nRetVal = g_Context.WaitAnyUpdateAll();	
	
}

xn::Context * ofxOpenNIContext::xnContext(){
	return &g_Context;
}
ofxOpenNIContext::~ofxOpenNIContext(){
	g_Context.Shutdown();
}