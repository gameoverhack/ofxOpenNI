/*
 *  ofxOpenNI.h
 *  opennisample
 *
 *  Created by Jonas Jongejan on 06/01/11.
 *
 */
#pragma once

#include "ofMain.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>


class ofxOpenNIContext {
public:
	ofxOpenNIContext();
	~ofxOpenNIContext();	

	bool setup();
	void update();
	
	xn::Context * xnContext();
	
private:
	xn::Context g_Context;
//	xn::UserGenerator g_UserGenerator;
	
};