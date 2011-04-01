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


