/*
 * ofxTrackedHand.cpp
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

#include "ofxTrackedHand.h"

//--------------------------------------------------------------
//
// HANDS
//
//--------------------------------------------------------------

// ctor
//--------------------------------------------------------------
ofxTrackedHand::ofxTrackedHand(ofxOpenNIContext* pContext)
{
	pContext->getDepthGenerator(&depth_generator);
	
	XnMapOutputMode map_mode;
	depth_generator.GetMapOutputMode(map_mode);
	
	xres = map_mode.nXRes;
	yres = map_mode.nYRes;
	zres = depth_generator.GetDeviceMaxDepth();;
	
	setFilterFactor(0.20f);		// set default??
	
	isBeingTracked = false;
}

// dtor
//--------------------------------------------------------------
ofxTrackedHand::~ofxTrackedHand() {
 // nothing
}

//--------------------------------------------------------------
// High-pass filter based on iOS Accelerometer sample
// Filter: 0.01 .. 1.0
//	Lower  = less noise, less speed
//	Higher = more noise, more speed
//#define FILTER_FACTOR		0.20

void ofxTrackedHand::update(const XnPoint3D* pPosition, bool filter, bool force) {
	
	rawPos = *pPosition;
	XnPoint3D rawProj = rawPos;
	depth_generator.ConvertRealWorldToProjective(1, &rawProj, &rawProj);
	
	if (filter && !force)
	{
		progPos.x = ( (rawProj.X / xres) * filter_factor) + (progPos.x * (1.0 - filter_factor));
		progPos.y = ( (rawProj.Y / yres) * filter_factor) + (progPos.y * (1.0 - filter_factor));
		progPos.z = ( (rawProj.Z / zres) * filter_factor) + (progPos.z * (1.0 - filter_factor));
		projectPos.x = (progPos.x * xres);
		projectPos.y = (progPos.y * yres);
		projectPos.z = (progPos.z * zres);
	}
	else
	{
		projectPos = ofPoint(rawProj.X, rawProj.Y, rawProj.Z);
		progPos.x = (projectPos.x / xres);
		progPos.y = (projectPos.y / yres);
		progPos.z = (projectPos.z / zres);
	}
}

//--------------------------------------------------------------
void ofxTrackedHand::setFilterFactor(float factor) {
	if (factor > 0 && factor <= 1.0) filter_factor = factor; // is this correct min and max?
}

//--------------------------------------------------------------
float ofxTrackedHand::getFilterFactor() {
	return filter_factor;
}

//--------------------------------------------------------------
void ofxTrackedHand::draw() {
	if (!isBeingTracked)
		return;
	ofFill();
	ofSetColor(255, 255, 0);
	ofCircle(projectPos.x, projectPos.y, 15);
}


