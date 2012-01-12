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
ofxOpenNIContext::ofxOpenNIContext(){
    //ofLogNotice(LOG_NAME) << "Creating ofxOpenNI context singleton";
    g_Context = NULL;
	bIsContextReady = false;
}

//--------------------------------------------------------------
ofxOpenNIContext::~ofxOpenNIContext(){
	ofLogNotice(LOG_NAME) << "Shutting down ofxOpenNI context singleton";
	stopThread();
    g_Depth.clear();
    g_Image.clear();
    g_IR.clear();
    g_Audio.clear();
    g_Device.clear();
	g_Context.Release();
}

//--------------------------------------------------------------
bool ofxOpenNIContext::initContext(){
	
    if (bIsContextReady) return true;
    
    ofLogNotice(LOG_NAME) << "Setting up ofxOpenNI context without XML...";
    
	XnStatus nRetVal = XN_STATUS_OK;
	string path = ofToDataPath(""); // hack warning: seems we need to call ofSetDataPathRoot(...) before a Context can init in 007!!
	
	nRetVal = g_Context.Init();
	SHOW_RC(nRetVal, "OpenNI Context initilized");
    bIsContextReady = (nRetVal == XN_STATUS_OK);
    
    addLicence("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
    
	if (getNumDevices() > 0){
		ofLogNotice(LOG_NAME) << "OpenNI Context initilized with" << getNumDevices() << "devices";
        startThread(true, false);
	} else {
		ofLogError(LOG_NAME) << "No devices found!!!";
	}
    
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNIContext::initDevices(){
    int numDevices = 0;
    if (initContext()){
        NodeInfoList list;
        numDevices = enumerateXnNode(XN_NODE_TYPE_DEVICE, list);
        for (int nodeIndex = 0; nodeIndex < numDevices; nodeIndex++) {
            addDeviceNode(nodeIndex);
        }
    }
    return numDevices;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addDeviceNode(int deviceID){
    int originalSize = g_Device.size();
    if (g_Device.size() < deviceID + 1) g_Device.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_DEVICE, g_Device[deviceID], deviceID);
    if (!ok) g_Device.resize(originalSize);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addDepthNode(int deviceID){
    int originalSize = g_Depth.size();
    if (g_Depth.size() < deviceID + 1) g_Depth.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_DEPTH, g_Depth[deviceID], deviceID);
    if (!ok) g_Depth.resize(originalSize);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addImageNode(int deviceID){
    int originalSize = g_Image.size();
    if (g_Image.size() < deviceID + 1) g_Image.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_IMAGE, g_Image[deviceID], deviceID);
    if (!ok) g_Image.resize(originalSize);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addInfraNode(int deviceID){
    int originalSize = g_IR.size();
    if (g_IR.size() < deviceID + 1) g_IR.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_IR, g_IR[deviceID], deviceID);
    if (!ok) g_IR.resize(originalSize);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addAudioNode(int deviceID){
    int originalSize = g_Audio.size();
    if (g_Audio.size() < deviceID + 1) g_Audio.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_AUDIO, g_Audio[deviceID], deviceID);
    if (!ok) g_Audio.resize(originalSize);
    return ok;
}

//--------------------------------------------------------------
int ofxOpenNIContext::enumerateXnNode(XnProductionNodeType type, NodeInfoList & list){
	
    XnStatus nRetVal = XN_STATUS_OK;
	EnumerationErrors errors;
    
	nRetVal = g_Context.EnumerateProductionTrees(type, NULL, list, &errors);
    
    SHOW_RC(nRetVal, "Enumerated node type: " + getNodeTypeAsString(type));
	if(nRetVal != XN_STATUS_OK) logErrors(errors);
    
	int nodeCount = 0;
	for (NodeInfoList::Iterator it = list.Begin(); it != list.End(); ++it){
		nodeCount++;
	}
    
	return nodeCount;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::createXnNode(XnProductionNodeType type, ProductionNode & node, int nodeIndex){
	
    XnStatus nRetVal = XN_STATUS_OK;
    NodeInfoList list;
	EnumerationErrors errors;
    
	int nodeCount = enumerateXnNode(type, list);

    if (nodeIndex + 1 > nodeCount){
        ofLogError(LOG_NAME) << "Requested nodeIndex" << nodeIndex << "> total nodeCount for" << getNodeTypeAsString(type);
        return false;
    }
    
    NodeInfoList::Iterator it = list.Begin();
    for (int i = 0; i < nodeIndex; i++) it++;
    
    NodeInfo nInfo = *it;
    nInfo.GetInstance(node);
    
    XnBool bExists = node.IsValid();
    if(!bExists){
        nRetVal = g_Context.CreateProductionTree(nInfo, node);
        SHOW_RC(nRetVal, "Creating node " + (string)node.GetName());
    }
    return (nRetVal == XN_STATUS_OK);
}


//--------------------------------------------------------------
void ofxOpenNIContext::threadedFunction(){
	while(isThreadRunning()){
		XnStatus nRetVal = XN_STATUS_OK;
		//lock(); // if I lock here application framerate drops to ~120fps
        nRetVal = g_Context.WaitAnyUpdateAll();
        CHECK_ERR_RC(nRetVal, "Error on WaitAnyUpdateAll()");
		//unlock();
	}
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addLicence(string sVendor, string sKey){
    ofLogNotice(LOG_NAME) << "Adding licence...";
	XnLicense license;
	XnStatus nRetVal = XN_STATUS_OK;
	bool ok = true;
    nRetVal = xnOSStrNCopy(license.strVendor, sVendor.c_str(),sVendor.size(), sizeof(license.strVendor));
    ok = (nRetVal == XN_STATUS_OK);
    CHECK_ERR_RC(nRetVal, "Error creating vendor: " + sVendor);
    nRetVal = xnOSStrNCopy(license.strKey, sKey.c_str(), sKey.size(), sizeof(license.strKey));
    ok = (nRetVal == XN_STATUS_OK);
    CHECK_ERR_RC(nRetVal, "Error creating key: " + sKey);	
    nRetVal = g_Context.AddLicense(license);
    ok = (nRetVal == XN_STATUS_OK);
    SHOW_RC(nRetVal, "Adding licence: " + sVendor + " " + sKey);
    xnPrintRegisteredLicenses();
    return ok;
}

//--------------------------------------------------------------
void ofxOpenNIContext::setLogLevel(XnLogSeverity logLevel){
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnLogSetConsoleOutput(true);
	SHOW_RC(nRetVal, "Set log to console");
	nRetVal = xnLogSetSeverityFilter(logLevel);
	SHOW_RC(nRetVal, "Set log level: " + logLevel);
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);
}

//--------------------------------------------------------------
void ofxOpenNIContext::logErrors(xn::EnumerationErrors & errors){
	for(xn::EnumerationErrors::Iterator it = errors.Begin(); it != errors.End(); ++it){
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc, 512);
		ofLog(OF_LOG_ERROR, "%s failed: %s\n", desc, xnGetStatusString(it.Error()));
	}	
}

//--------------------------------------------------------------
bool ofxOpenNIContext::getIsContextReady(){
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNIContext::getNumDevices(){
    if (g_Device.size() == 0) {
        initDevices();
    }
	return g_Device.size();
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