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

#include <stdio.h>
#include <stdlib.h>
#include "ofxOpenNIContext.h"

string ofxOpenNIContext::LOG_NAME = "ofxOpenNIContext";

// need this because when using xn::UserGenerator's I'm getting SIGSEV's on exit at WaitAnyUpdateAll...
// ... it's proving very difficult to get a lock and to destroy nodes etc ...
// ... this solution only works about 50% of the time, as we often get crash BEFORE dtors are called
// AND before even the aexit() functions ... after a day and half I'm bored with it!!!
static void onExitKillContext(){
    cout << "killing the context on exit";
    openNIContext->setPaused(true);
    openNIContext->lock();
    openNIContext->stopThread();
    openNIContext->getContext().StopGeneratingAll();
    openNIContext->unlock();
}

//--------------------------------------------------------------
ofxOpenNIContext::ofxOpenNIContext(){
    //ofLogNotice(LOG_NAME) << "Creating ofxOpenNI context singleton";
    g_Context = NULL;
	bIsContextReady = false;
}


//--------------------------------------------------------------
ofxOpenNIContext::~ofxOpenNIContext(){
	
    bPaused = true;
    
    g_Context.StopGeneratingAll();
    
    if (isThreadRunning()){
        lock();
        stopThread();
	}
    
    if (g_Depth.size() > 0) g_Depth.clear();
    if (g_Image.size() > 0) g_Image.clear();
    if (g_IR.size() > 0) g_IR.clear();
    if (g_User.size() > 0) g_User.clear();
    if (g_Audio.size() > 0) g_Audio.clear();
    if (g_Device.size() > 0) g_Device.clear();
    
    //g_Context.Shutdown();
    //g_Context.Release();
    
     ofLogNotice(LOG_NAME) << "Shut down ofxOpenNI context singleton"; // strange if I call at top of dtor I crash!?!?!
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
    
    bPaused = true;
    
	if (getNumDevices() > 0){
		ofLogNotice(LOG_NAME) << "OpenNI Context initilized with" << getNumDevices() << "devices";
        startThread(true, false);
        std::atexit (onExitKillContext); // see not above about exit/crash/frustration!
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
    if (ok) bPaused = false;
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addDepthNode(int deviceID){
    int originalSize = g_Depth.size();
    if (g_Depth.size() < deviceID + 1) g_Depth.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_DEPTH, g_Depth[deviceID], deviceID);
    if (!ok) g_Depth.resize(originalSize);
    if (ok) bPaused = false;
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addImageNode(int deviceID){
    int originalSize = g_Image.size();
    if (g_Image.size() < deviceID + 1) g_Image.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_IMAGE, g_Image[deviceID], deviceID);
    if (!ok) g_Image.resize(originalSize);
    if (ok) bPaused = false;
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addInfraNode(int deviceID){
    int originalSize = g_IR.size();
    if (g_IR.size() < deviceID + 1) g_IR.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_IR, g_IR[deviceID], deviceID);
    if (!ok) g_IR.resize(originalSize);
    if (ok) bPaused = false;
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addUserNode(int deviceID){
    int originalSize = g_User.size();
    if (g_User.size() < deviceID + 1) g_User.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_USER, g_User[deviceID], deviceID);
    if (!ok) g_User.resize(originalSize);
    if (ok) bPaused = false;
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNIContext::addAudioNode(int deviceID){
    int originalSize = g_Audio.size();
    if (g_Audio.size() < deviceID + 1) g_Audio.resize(deviceID + 1);
    bool ok = createXnNode(XN_NODE_TYPE_AUDIO, g_Audio[deviceID], deviceID);
    if (!ok) g_Audio.resize(originalSize);
    if (ok) bPaused = false;
    return ok;
}

bool ofxOpenNIContext::stopDepthNode(int deviceID){
    if (deviceID > g_Depth.size()) return false;
    XnStatus nRetVal = XN_STATUS_OK;
    //Poco::ScopedLock<ofMutex> lock(mutex);
    nRetVal = g_Depth[deviceID].StopGenerating();
    SHOW_RC(nRetVal, "Stop generating depth");
    g_Depth[deviceID].Release();
    return (nRetVal == XN_STATUS_OK);
}

//--------------------------------------------------------------
bool ofxOpenNIContext::stopImageNode(int deviceID){
    if (deviceID > g_Image.size()) return false;
    XnStatus nRetVal = XN_STATUS_OK;
    //Poco::ScopedLock<ofMutex> lock(mutex);
    nRetVal = g_Image[deviceID].StopGenerating();
    SHOW_RC(nRetVal, "Stop generating image");
    g_Image[deviceID].Release();
    return (nRetVal == XN_STATUS_OK);
}

//--------------------------------------------------------------
bool ofxOpenNIContext::stopInfraNode(int deviceID){
    if (deviceID > g_IR.size()) return false;
    XnStatus nRetVal = XN_STATUS_OK;
    //Poco::ScopedLock<ofMutex> lock(mutex);
    nRetVal = g_IR[deviceID].StopGenerating();
    SHOW_RC(nRetVal, "Stop generating infra");
    g_IR[deviceID].Release();
    return (nRetVal == XN_STATUS_OK);
}

//--------------------------------------------------------------
bool ofxOpenNIContext::stopUserNode(int deviceID){
    if (deviceID > g_User.size()) return false;
    XnStatus nRetVal = XN_STATUS_OK;
    //Poco::ScopedLock<ofMutex> lock(mutex);
    nRetVal = g_User[deviceID].StopGenerating();
    SHOW_RC(nRetVal, "Stop generating user");
    g_User[deviceID].Release();
    return (nRetVal == XN_STATUS_OK);
}

//--------------------------------------------------------------
bool ofxOpenNIContext::stopAudioNode(int deviceID){
    if (deviceID > g_Audio.size()) return false;
    XnStatus nRetVal = XN_STATUS_OK;
    //Poco::ScopedLock<ofMutex> lock(mutex);
    nRetVal = g_Audio[deviceID].StopGenerating();
    SHOW_RC(nRetVal, "Stop generating audio");
    g_Audio[deviceID].Release();
    return (nRetVal == XN_STATUS_OK);
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
    
    ofLogVerbose(LOG_NAME) << "Found" << nodeCount << "nodes of type:" << getNodeTypeAsString(type);
    
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
        SHOW_RC(nRetVal, "Creating node " + (string)node.GetName() + " with nodeIndex " + ofToString(nodeIndex));
    }
    
    return (nRetVal == XN_STATUS_OK);
}

//--------------------------------------------------------------
bool ofxOpenNIContext::getPaused(){
    return bPaused;
}

//--------------------------------------------------------------
void ofxOpenNIContext::setPaused(bool b){
    bPaused = true;
}

//--------------------------------------------------------------
void ofxOpenNIContext::togglePaused(){
    bPaused = !bPaused;
}

//--------------------------------------------------------------
void ofxOpenNIContext::threadedFunction(){
	while(isThreadRunning()){
        if (!bPaused) {
            lock();
            XnStatus nRetVal = XN_STATUS_OK;
            nRetVal = g_Context.WaitAnyUpdateAll();
            CHECK_ERR_RC(nRetVal, "Error on WaitAnyUpdateAll()");
            unlock();
        }
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
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_Context;
}

//--------------------------------------------------------------
xn::Device& ofxOpenNIContext::getDevice(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_Device[deviceID];
}

//--------------------------------------------------------------
xn::DepthGenerator& ofxOpenNIContext::getDepthGenerator(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_Depth[deviceID];
}

//--------------------------------------------------------------
xn::ImageGenerator& ofxOpenNIContext::getImageGenerator(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_Image[deviceID];
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNIContext::getIRGenerator(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_IR[deviceID];
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNIContext::getAudioGenerator(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_Audio[deviceID];
}

//--------------------------------------------------------------
xn::UserGenerator& ofxOpenNIContext::getUserGenerator(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_User[deviceID];
}

//--------------------------------------------------------------
xn::Player& ofxOpenNIContext::getPlayer(int deviceID){
    //Poco::ScopedLock<ofMutex> lock(mutex);
	return g_Player[deviceID];
}