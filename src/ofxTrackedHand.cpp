#include "ofxTrackedHand.h"

//--------------------------------------------------------------
//
// HANDS
//
//--------------------------------------------------------------

//--------------------------------------------------------------
ofxTrackedHand::ofxTrackedHand(ofxOpenNIContext* pContext)
{
	pContext->getDepthGenerator(&depth_generator);
	
	XnMapOutputMode map_mode;
	depth_generator.GetMapOutputMode(map_mode);
	
	xres = map_mode.nXRes;
	yres = map_mode.nYRes;
	zres = depth_generator.GetDeviceMaxDepth();;
	
	isBeingTracked = false;
}

//--------------------------------------------------------------
// High-pass filter based on iOS Accelerometer sample
// Filter: 0.01 .. 1.0
//	Lower  = less noise, less speed
//	Higher = more noise, more speed
#define FILTER_FACTOR		0.20
void ofxTrackedHand::update(const XnPoint3D* pPosition, bool filter, bool force) {
	
	rawPos = *pPosition;
	XnPoint3D rawProj = rawPos;
	depth_generator.ConvertRealWorldToProjective(1, &rawProj, &rawProj);
	
	
	if (filter && !force)
	{
		progPos.x = ( (rawProj.X / xres) * FILTER_FACTOR) + (progPos.x * (1.0 - FILTER_FACTOR));
		progPos.y = ( (rawProj.Y / yres) * FILTER_FACTOR) + (progPos.y * (1.0 - FILTER_FACTOR));
		progPos.z = ( (rawProj.Z / zres) * FILTER_FACTOR) + (progPos.z * (1.0 - FILTER_FACTOR));
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
void ofxTrackedHand::draw() {
	if (!isBeingTracked)
		return;
	ofFill();
	ofSetColor(255, 255, 0);
	ofCircle(projectPos.x, projectPos.y, 15);
}


