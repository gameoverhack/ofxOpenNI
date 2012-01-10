/*
 * ofxOpenNIContext.cpp
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

#include "ofxOpenNIContext.h"

string ofxOpenNIContext::LOG_NAME = "ofxOpenNIContext";

//--------------------------------------------------------------
ofxOpenNIContext::ofxOpenNIContext() {
    //ofLogVerbose(LOG_NAME) << "Creating ofxOpenNI context singleton";
	bIsContextReady = false;
	g_pPrimary = NULL;
}

//--------------------------------------------------------------
ofxOpenNIContext::~ofxOpenNIContext() {
	ofLogVerbose(LOG_NAME) << "Shutting down ofxOpenNI context singleton";
	stopThread();
	for (int deviceID = 0; deviceID < numDevices; deviceID++) {
		g_Depth[deviceID].Release();
		g_Image[deviceID].Release();
		g_IR[deviceID].Release();
		g_User[deviceID].Release();
		g_Audio[deviceID].Release();
	}
	g_Context.Release();
}

//--------------------------------------------------------------
bool ofxOpenNIContext::initContext() {
	
	ofLogWarning(LOG_NAME) << "THIS IS EXPERIMENTAL CODE!! USES A SINGLE CONEXT FOR MULTIPLE DEVICES. THIS WORKS BUT SEEMS ODD...";
	
	XnStatus nRetVal;
	
	//setLogLevel(XN_LOG_VERBOSE);

	string path = ofToDataPath(""); // crazily this is necessary or else enumeration of devices fails!!!
                                    // took me hours/days to figure it out...but for some reason the
                                    // oF rootPath needs to be set it would seem...
	
	nRetVal = g_Context.Init();
	ofLogVerbose(LOG_NAME) << "OpenNI Context initilized:" << xnGetStatusString(nRetVal);

	addLicence("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");

	numDevices = enumerateAndCreateXnNode(XN_NODE_TYPE_DEVICE, g_Device);
	
	if (numDevices > 0) {
		enumerateAndCreateXnNode(XN_NODE_TYPE_DEPTH, g_Depth);
		enumerateAndCreateXnNode(XN_NODE_TYPE_IMAGE, g_Image);
		enumerateAndCreateXnNode(XN_NODE_TYPE_IR, g_IR);
		enumerateAndCreateXnNode(XN_NODE_TYPE_USER, g_User);
		enumerateAndCreateXnNode(XN_NODE_TYPE_AUDIO, g_Audio);
	} else {
		ofLogError(LOG_NAME) << "No devices found!!!";
	}
	
	startThread(true, false);
	
	if (nRetVal == XN_STATUS_OK) {
		bIsContextReady = true;
	} else {
		bIsContextReady =  false;
	}
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNIContext::enumerateAndCreateXnNode(XnProductionNodeType type, ProductionNode* nodes) {
	
	NodeInfoList list;
	xn::EnumerationErrors errors;
	XnStatus nRetVal;
	
	ofLogVerbose(LOG_NAME) << "Enumerating" << getNodeTypeAsString(type) << "nodes...";
	nRetVal = g_Context.EnumerateProductionTrees(type, NULL, list, &errors);
	ofLogVerbose(LOG_NAME) << "Enumeration:" << xnGetStatusString(nRetVal);
	
	if(nRetVal != XN_STATUS_OK) logErrors(errors);

	int nodeIndex = 0;
	for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it, ++nodeIndex) {
		NodeInfo nInfo = *it;
		
		nInfo.GetInstance(nodes[nodeIndex]);
		XnBool bExists = nodes[nodeIndex].IsValid();
		
		if(!bExists) {
			nRetVal = g_Context.CreateProductionTree(nInfo, nodes[nodeIndex]);
			ofLogVerbose(LOG_NAME) << "Creating node" << xnGetStatusString(nRetVal) << nodeIndex << nodes[nodeIndex].GetName();
		}
		
	}
	return nodeIndex;
}

//--------------------------------------------------------------
void ofxOpenNIContext::threadedFunction(){
	while(isThreadRunning()){
		XnStatus nRetVal = XN_STATUS_OK;
		//lock(); // if I lock here application framerate drops to ~120fps
		if (g_pPrimary != NULL){
			nRetVal = g_Context.WaitOneUpdateAll(*g_pPrimary);
		}else{
			nRetVal = g_Context.WaitAnyUpdateAll();
		}
		
		if (nRetVal != XN_STATUS_OK){
			ofLogError(LOG_NAME) << "Error on WaitAnyUpdateAll():" << xnGetStatusString(nRetVal);
			return;
		}
		//unlock();
	}
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addLicence(string sVendor, string sKey) {
	
	XnLicense license = {0};
	XnStatus nRetVal = XN_STATUS_OK;
	bool ok = true;
	
	nRetVal = xnOSStrNCopy(license.strVendor, sVendor.c_str(),sVendor.size(), sizeof(license.strVendor));
	if(nRetVal != XN_STATUS_OK) {
		ofLogError(LOG_NAME) << "Error creating vendor:" << sVendor;
		ok = false;
	}
	
	nRetVal = xnOSStrNCopy(license.strKey, sKey.c_str(), sKey.size(), sizeof(license.strKey));
	if(nRetVal != XN_STATUS_OK) {
		ofLogError(LOG_NAME) << "Error creating keyy:" << sKey;
		ok = false;
	}	
	
	nRetVal = g_Context.AddLicense(license);
	ofLogVerbose(LOG_NAME) << "Adding licence:" << sVendor << sKey << xnGetStatusString(nRetVal);
	
	if(nRetVal != XN_STATUS_OK) ok = false;
	
	return ok;
	
	//xnPrintRegisteredLicenses();
}

//--------------------------------------------------------------
void ofxOpenNIContext::setLogLevel(XnLogSeverity logLevel) {
	
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnLogSetConsoleOutput(true);
	
	ofLogVerbose(LOG_NAME) << "Set log to console:" << xnGetStatusString(nRetVal);
	
	nRetVal = xnLogSetSeverityFilter(logLevel);	// TODO: set different log levels with code; enable and disable functionality
	ofLogVerbose(LOG_NAME) << "Set log level:" << xnGetStatusString(nRetVal) << logLevel;
	
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);
	
}

//--------------------------------------------------------------
void ofxOpenNIContext::logErrors(xn::EnumerationErrors & errors){
	for(xn::EnumerationErrors::Iterator it = errors.Begin(); it != errors.End(); ++it) {
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc,512);
		ofLog(OF_LOG_ERROR, "%s failed: %s\n", desc, xnGetStatusString(it.Error()));
	}	
}

//--------------------------------------------------------------
bool ofxOpenNIContext::getIsContextReady(){
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNIContext::getNumDevices() {
	return numDevices;
}

//--------------------------------------------------------------
xn::Context& ofxOpenNIContext::getContext(){
	return g_Context;
}

//--------------------------------------------------------------
xn::Device& ofxOpenNIContext::getDevice(int deviceID){
	return g_Device[deviceID];
}

//--------------------------------------------------------------
xn::DepthGenerator& ofxOpenNIContext::getDepthGenerator(int deviceID){
	return g_Depth[deviceID];
}

//--------------------------------------------------------------
xn::ImageGenerator& ofxOpenNIContext::getImageGenerator(int deviceID){
	return g_Image[deviceID];
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNIContext::getIRGenerator(int deviceID){
	return g_IR[deviceID];
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNIContext::getAudioGenerator(int deviceID){
	return g_Audio[deviceID];
}

//--------------------------------------------------------------
xn::Player& ofxOpenNIContext::getPlayer(int deviceID){
	return g_Player[deviceID];
}