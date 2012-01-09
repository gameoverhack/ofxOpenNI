/*
 * ofxTrackedHand.h
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

#ifndef _H_OFXTRACKEDHAND
#define _H_OFXTRACKEDHAND

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include "ofxOpenNIContext.h"
#include "ofMain.h"

// This is ready for more hands, but.....
// If MAX_NUMBER_HANDS > 1, gesture triggers several Gesture_Recognized callbacks
// So, to detect more hands, we need to figure out how to identify if a gesture
// is from the same hand or from another hand.
// Strange thing is, if MAX_NUMBER_HANDS is 1, Gesture_Recognized fires only once

class ofxOpenNIContext;
class ofxTrackedHand {

public:

	ofxTrackedHand();
	~ofxTrackedHand();

	void update(const XnPoint3D* pPosition, bool filter=false, bool force=false);

	void draw();

	bool				isBeingTracked;
	XnUserID			nID;
	XnPoint3D			rawPos;
	ofPoint				projectPos;		// position on screen
	ofPoint				progPos;		// position from 0.0 to 1.0

	void				setFilterFactor(float factor);
	float				getFilterFactor();

private:

	ofxTrackedHand(ofxOpenNIContext* pContext);

	xn::DepthGenerator	depth_generator;

	float				xres, yres, zres;
	float				filter_factor;

	friend class ofxHandGenerator;
};

#endif
