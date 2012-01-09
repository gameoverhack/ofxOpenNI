/*
 * ofxOpenNIContext.cpp
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

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxOpenNIMacros.h"

// Startup
//----------------------------------------
ofxOpenNIContext::ofxOpenNIContext() {
    string path = ofToDataPath(""); // hack needed to set root path in 007 otherwise openNI isn't working!?!
	is_using_recording = false;
}

// Just initialize; use this when you"re creating nodes yourself.
//----------------------------------------
bool ofxOpenNIContext::initContext(){
	xn::EnumerationErrors errors;
	XnStatus result = context.Init();
	if(result != XN_STATUS_OK) logErrors(errors);
	BOOL_RC(result, "ofxOpenNIContext.setup()");
}

// Initialize using an .ONI recording.
//----------------------------------------
bool ofxOpenNIContext::setupUsingRecording(std::string sFileRecording) {
	
	xn::EnumerationErrors errors;
	
	initContext();
	addLicense("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
	
	is_using_recording = true;
	
	std::string file_path = ofToDataPath(sFileRecording.c_str(), true);
	
	printf("Attempting to open file: %s\n", file_path.c_str());
	
	XnStatus result = context.OpenFileRecording(file_path.c_str());
	
	if(result != XN_STATUS_OK) logErrors(errors);
	
	BOOL_RC(result, "Loading file");
}

void ofxOpenNIContext::logErrors(xn::EnumerationErrors& rErrors) {
	for(xn::EnumerationErrors::Iterator it = rErrors.Begin(); it != rErrors.End(); ++it) {
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc,512);
		printf("%s failed: %s\n", desc, xnGetStatusString(it.Error()));
	}	
}

// Initialize using code only
bool ofxOpenNIContext::setup() {
	
	if (initContext()) {
		addLicense("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
		enableLogging();
		return true;
	} else return false;

}

// Initialize using an XML file.
//----------------------------------------
bool ofxOpenNIContext::setupUsingXMLFile(std::string sFile) {
	
	xn::EnumerationErrors errors;
	
	if(sFile == "") sFile = ofToDataPath("openni/config/ofxopenni_config.xml",true);
	
	printf("Using file: %s\n", sFile.c_str());
	
	XnStatus result = context.InitFromXmlFile(sFile.c_str(), &errors);
	
	if(result != XN_STATUS_OK) logErrors(errors);
	
	BOOL_RC(result, "ofxOpenNIContext.setupUsingXMLFile()");
	
}

// This is used by other nodes (ofxDepthGenerator), which need to 
// use a different initialization when you"re loading an recording.
//----------------------------------------
bool ofxOpenNIContext::isUsingRecording() {
	return is_using_recording;
}

// Use these to retrieve references to various node types on the production tree
// TODO: should these be made static so retrievl is more immediate? Or is this sufficient?
//----------------------------------------
bool ofxOpenNIContext::getDepthGenerator(xn::DepthGenerator* depth_generator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(XN_NODE_TYPE_DEPTH, *depth_generator);
	BOOL_RC(result, "Retrieving depth generator");
}

bool ofxOpenNIContext::getImageGenerator(xn::ImageGenerator* image_generator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(XN_NODE_TYPE_IMAGE, *image_generator);
	BOOL_RC(result, "Retrieving image generator");
}

bool ofxOpenNIContext::getIRGenerator(xn::IRGenerator* ir_generator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(XN_NODE_TYPE_IR, *ir_generator);
	BOOL_RC(result, "Retrieving ir generator");
}

bool ofxOpenNIContext::getUserGenerator(xn::UserGenerator* user_generator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(XN_NODE_TYPE_USER, *user_generator);
	BOOL_RC(result, "Retrieving user generator");
}

bool ofxOpenNIContext::getGestureGenerator(xn::GestureGenerator* gesture_generator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(XN_NODE_TYPE_GESTURE, *gesture_generator);
	BOOL_RC(result, "Retrieving gesture generator");
}

bool ofxOpenNIContext::getHandsGenerator(xn::HandsGenerator* hands_generator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(XN_NODE_TYPE_HANDS, *hands_generator);
	BOOL_RC(result, "Retrieving hands generator");
}

// we need to programmatically add a license when playing back a recording
// file otherwise the skeleton tracker will throw an error and not work
void ofxOpenNIContext::addLicense(std::string sVendor, std::string sKey) {
	
	XnLicense license = {0};
	XnStatus status = XN_STATUS_OK;
	
	status = xnOSStrNCopy(license.strVendor, sVendor.c_str(),sVendor.size(), sizeof(license.strVendor));
	if(status != XN_STATUS_OK) {
		printf("ofxOpenNIContext error creating license (vendor)\n");
		return;
	}
	
	status = xnOSStrNCopy(license.strKey, sKey.c_str(), sKey.size(), sizeof(license.strKey));
	if(status != XN_STATUS_OK) {
		printf("ofxOpenNIContext error creating license (key)\n");
		return;
	}	
	
	status = context.AddLicense(license);
	SHOW_RC(status, "AddLicense");
	
	xnPrintRegisteredLicenses();
	
}

// TODO: check this is working and use it with ONI recordings??
void ofxOpenNIContext::enableLogging() {
	
	XnStatus result = xnLogSetConsoleOutput(true);
	SHOW_RC(result, "Set console output");
	
	result = xnLogSetSeverityFilter(XN_LOG_ERROR);	// TODO: set different log levels with code; enable and disable functionality
	SHOW_RC(result, "Set log level");

	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);
	
}

// Update all nodes, should be call in the ofTestApp::update()
//----------------------------------------
void ofxOpenNIContext::update(){
	XnStatus nRetVal = context.WaitAnyUpdateAll();	
}

// Allow us to mirror the image_gen/depth_gen
bool ofxOpenNIContext::toggleMirror() {
	return setMirror(!context.GetGlobalMirror());
}

bool ofxOpenNIContext::setMirror(XnBool mirroring) {
	XnStatus result = context.SetGlobalMirror(mirroring);
	BOOL_RC(result, "Set mirroring");
}

bool ofxOpenNIContext::toggleRegisterViewport() {
	
	// get refs to depth and image generators TODO: make work with IR generator
	xn::DepthGenerator depth_generator;
	getDepthGenerator(&depth_generator);
	
	xn::ImageGenerator image_generator;
	if (!getImageGenerator(&image_generator)) {
		printf("No Image generator found: cannot register viewport");
		return false;
	}
	
	// Toggle registering view point to image map
	if (depth_generator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT))
	{
		
		if(depth_generator.GetAlternativeViewPointCap().IsViewPointAs(image_generator)) {
			unregisterViewport();
		} else {
			registerViewport();
		}
		
	} else return false;
	
	return true;
}

bool ofxOpenNIContext::registerViewport() {
	
	// get refs to depth and image generators TODO: make work with IR generator
	xn::DepthGenerator depth_generator;
	getDepthGenerator(&depth_generator);
	
	xn::ImageGenerator image_generator;
	if (!getImageGenerator(&image_generator)) {
		printf("No Image generator found: cannot register viewport");
		return false;
	}
	
	// Register view point to image map
	if (depth_generator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		
		XnStatus result = depth_generator.GetAlternativeViewPointCap().SetViewPoint(image_generator);
		CHECK_RC(result, "Register viewport");
		
	} else return false;
	
	return true;
}

bool ofxOpenNIContext::unregisterViewport() {
	
	// get refs to depth generator
	xn::DepthGenerator depth_generator;
	getDepthGenerator(&depth_generator);
	
	// Unregister view point from (image) any map	
	if (depth_generator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		XnStatus result = depth_generator.GetAlternativeViewPointCap().ResetViewPoint();
		CHECK_RC(result, "Unregister viewport");
		
	} else return false;
	
	return true;
}

// Get a reference to the xn::Context.
//----------------------------------------
xn::Context& ofxOpenNIContext::getXnContext(){
	return context;
}

void ofxOpenNIContext::shutdown() {
	printf("Shutdown context\n");
	context.Shutdown();
}

// Shutdown.
//----------------------------------------
ofxOpenNIContext::~ofxOpenNIContext(){
	shutdown();
}