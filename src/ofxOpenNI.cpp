/*
 * ofxOpenNI.cpp
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

#include "ofxOpenNI.h"

static int instanceCount = -1;

/**************************************************************
 *
 *      constructor and setup methods
 *
 *************************************************************/

//--------------------------------------------------------------
ofxOpenNI::ofxOpenNI(){
	instanceCount++;
	instanceID = instanceCount; // TODO: this should be replaced/combined with a listDevices and setDeviceID methods
    
    LOG_NAME = "ofxOpenNIDevice[" + ofToString(instanceID) + "]";
    //cout << LOG_NAME << ": constructor called" << endl;
    
	bIsThreaded = false;
	g_bIsDepthOn = false;
	g_bIsDepthRawOnOption = false;
	g_bIsImageOn = false;
	g_bIsInfraOn = false;
    g_bIsUserOn = false;
    g_bIsGestureOn = false;
    g_bIsHandsOn = false;
	g_bIsAudioOn = false;
    g_bIsRecordOn = false;
	
	depthColoring = COLORING_RAINBOW;
	
    bIsContextReady = false;
    bIsDeviceReady = false;
    bIsShuttingDown = false;
    
    bUseRegistration = false;
    bUseSync = false;
    bUseMirror = false;
    
    bAutoCalibrationPossible = false;
    
    bUseBackBuffer = false;
	bUseTexture = true;
    bUseSafeThreading = false;
	bNewPixels = false;
	bNewFrame = false;
	
    width = XN_VGA_X_RES;
    height = XN_VGA_Y_RES;
    fps = 30;
    
    maxNumUsers = 1;
    minTimeBetweenGestures = 0;
    skeletonProfile = XN_SKEL_PROFILE_ALL;
    
    maxNumHands = 1;
    minTimeBetweenHands = 500;
    minDistanceBetweenHands = 100;

	CreateRainbowPallet();
    
    logLevel = OF_LOG_SILENT;
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(bool threaded){
    init("", "", threaded);
}

//--------------------------------------------------------------
bool ofxOpenNI::setupFromXML(string xmlFilePath, bool threaded){
    init("", xmlFilePath, threaded);
}

bool ofxOpenNI::setupFromONI(string oniFilePath, bool threaded){
    init(oniFilePath, "", threaded);
}

//--------------------------------------------------------------
bool ofxOpenNI::init(string oniFilePath, string xmlFilePath, bool threaded){
    setSafeThreading(bUseSafeThreading);
    
    XnStatus nRetVal = XN_STATUS_OK;
	bIsThreaded = threaded;
    
    // set log level to ofLogLevel base level at least
    if (ofGetLogLevel() < logLevel) logLevel = ofGetLogLevel();
    setLogLevel(logLevel);
    
	if (!initContext(xmlFilePath)){
        ofLogError(LOG_NAME) << "Context could not be initialized";
        return false;
	}
    
        
    if (oniFilePath != ""){
        if (!startPlayer(oniFilePath)){
            ofLogError(LOG_NAME) << "ONI Device could not be initialized";
            return false;
        }
    }
    if (oniFilePath != ""){
        if (!initCommon()){
            ofLogError(LOG_NAME) << "XML Device could not be initialized";
            return false;
        }
    }
    if (!initDevice()){
        ofLogWarning(LOG_NAME) << "REAL Device could not be initialized";
        //return false;
    }
    
//    if (!bIsDeviceReady || numDevices < instanceID) {
//        ofLogWarning(LOG_NAME) << "No Devices available for this instance [" << instanceID << "] of ofxOpenNI - have you got a device connected?";
//        //        g_Context.Release();
//        //        bIsContextReady = false;
//        //        return false;
//    }
    if (bIsThreaded && !isThreadRunning()) {
        ofLogNotice(LOG_NAME) << "Starting ofxOpenNI with threading";
        startThread(true, false);
    } else if (!bIsThreaded) {
        ofLogNotice(LOG_NAME) << "Starting ofxOpenNI without threading";
    }
    return bIsContextReady;
}

//--------------------------------------------------------------
bool ofxOpenNI::initContext(string xmlFilePath){
    if (bIsContextReady) return true;
    ofLogNotice(LOG_NAME) << "Init context...";
    XnStatus nRetVal = XN_STATUS_OK;
    string ofXmlFilePath = ofToDataPath(xmlFilePath); // of007 hack to make sure ofSetRootPath is done!
    if (xmlFilePath == ""){
        nRetVal = g_Context.Init();
        SHOW_RC(nRetVal, "Context initilized");
    }else{
        nRetVal = g_Context.InitFromXmlFile(ofXmlFilePath.c_str());
        SHOW_RC(nRetVal, "Context initilized from XML: " + ofXmlFilePath);
    }
    bIsContextReady = (nRetVal == XN_STATUS_OK);
    if (bIsContextReady) addLicence("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
    return bIsContextReady;
}

//--------------------------------------------------------------
bool ofxOpenNI::initDevice(){
    if (!bIsContextReady) initContext();
    if (bIsContextReady) {
        if (bIsDeviceReady) return true;
        ofLogNotice(LOG_NAME) << "Init device...";
        XnStatus nRetVal = XN_STATUS_OK;
        NodeInfoList deviceList;
        EnumerationErrors errors;
        
        nRetVal = g_Context.EnumerateProductionTrees (XN_NODE_TYPE_DEVICE, NULL, deviceList, &errors);
        SHOW_RC(nRetVal, "Enumerate devices");
        if(nRetVal != XN_STATUS_OK) logErrors(errors);
        
        numDevices = 0;
        NodeInfo nInfo = *deviceList.Begin();
        for (NodeInfoList::Iterator it = deviceList.Begin(); it != deviceList.End(); ++it, ++numDevices){
            if (numDevices == instanceID) { // ok this is brittle, maybe use a static vector to keep track of free devices??
                nInfo = *it;
            }
        }
        
        ofLogNotice(LOG_NAME) << "Found" << numDevices << "devices connected";
        
        if (numDevices > 0){
            nRetVal = g_Context.CreateProductionTree(nInfo, g_Device);
            SHOW_RC(nRetVal, "Creating production tree for device " + ofToString(instanceID));
        }
        bIsDeviceReady = (nRetVal == XN_STATUS_OK);
    } else {
        bIsDeviceReady = false;
    }
    return bIsDeviceReady;
}

//--------------------------------------------------------------
bool ofxOpenNI::initCommon(){
//    if (isThreadRunning()) waitForThread(true);
//    if (bIsContextReady) stopCommon();
    bool ok = true;
    XnStatus nRetVal = XN_STATUS_OK;
    NodeInfoList list;
	nRetVal = g_Context.EnumerateExistingNodes(list);
	if (nRetVal == XN_STATUS_OK){
        NodeInfoList::Iterator it = list.Begin();
        if (it == list.End()) return false;
		for (it = list.Begin(); it != list.End(); ++it){
			switch ((*it).GetDescription().Type){
                case XN_NODE_TYPE_DEVICE:
                    ofLogVerbose(LOG_NAME) << "Creating device from ONI/XML";
                    (*it).GetInstance(g_Device);
                    bIsDeviceReady = true;
                    if (numDevices == 0) numDevices = 1;
                    break;
                case XN_NODE_TYPE_DEPTH:
                    ofLogVerbose(LOG_NAME) << "Creating depth generator from ONI/XML";
                    g_bIsDepthOn = true;
                    //g_bIsDepthRawOnOption = true;
                    (*it).GetInstance(g_Depth);
                    if (!l_bIsDepthOn) allocateDepthBuffers();
                    break;
                case XN_NODE_TYPE_IMAGE:
                    ofLogVerbose(LOG_NAME) << "Creating image generator from ONI/XML";
                    g_bIsImageOn = true;
                    (*it).GetInstance(g_Image);
                    if (!l_bIsImageOn)allocateImageBuffers();
                    break;
                case XN_NODE_TYPE_IR:
                    ofLogVerbose(LOG_NAME) << "Creating ir generator from ONI/XML";
                    g_bIsInfraOn = true;
                    (*it).GetInstance(g_Infra);
                    if (!l_bIsInfraOn) allocateIRBuffers();
                    break;
                case XN_NODE_TYPE_AUDIO:
                    ofLogVerbose(LOG_NAME) << "Creating audio generator from ONI/XML";
                    g_bIsAudioOn = true;
                    (*it).GetInstance(g_Audio);
                    //if (!l_bIsAudioOn) allocateAudioBuffers();
                    break;
                case XN_NODE_TYPE_PLAYER:
                    ofLogVerbose(LOG_NAME) << "Creating player from ONI/XML";
                    g_bIsPlayerOn = true;
                    (*it).GetInstance(g_Player);
                    break;
			}
		}
	} else {
        ok = false;
    }
//    if (bIsThreaded) startThread(true, false);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::addLicence(string sVendor, string sKey){
    ofLogNotice(LOG_NAME) << "Adding licence...";
	XnLicense license;
	XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = xnOSStrNCopy(license.strVendor, sVendor.c_str(),sVendor.size(), sizeof(license.strVendor));
    CHECK_ERR_RC(nRetVal, "Error creating vendor: " + sVendor);
    nRetVal = xnOSStrNCopy(license.strKey, sKey.c_str(), sKey.size(), sizeof(license.strKey));
    CHECK_ERR_RC(nRetVal, "Error creating key: " + sKey);	
    nRetVal = g_Context.AddLicense(license);
    SHOW_RC(nRetVal, "Adding licence: " + sVendor + " " + sKey);
    //xnPrintRegisteredLicenses();
    return (nRetVal == XN_STATUS_OK);
}

/**************************************************************
 *
 *      destructor and stop methods
 *
 *************************************************************/

//--------------------------------------------------------------
ofxOpenNI::~ofxOpenNI(){
    setSafeThreading(true); // probably will not work but worth a try
    // don't use ofLog here!!!
    cout << LOG_NAME << ": destructor called" << endl;
    if (bIsShuttingDown) {
        cout << LOG_NAME << ": ...already shut down" << endl;
        return;
    }
    stop();
    bIsShuttingDown = true;
}

//--------------------------------------------------------------
void ofxOpenNI::stop(){
    setSafeThreading(true);
    // don't use ofLog here!!!
    cout << LOG_NAME << ": stop called" << endl;
    if (bIsShuttingDown) {
        cout << LOG_NAME << ": ...already shut down" << endl;
        return;
    }
    
    bIsShuttingDown = true;
    
    if (!bIsContextReady) return;
    
    if (bIsThreaded) {
        cout << LOG_NAME << ": trying to lock" << endl;
        lock();
        if (isThreadRunning()){
            cout << LOG_NAME << ": trying to stop thread" << endl;
            stopThread();
            bIsThreaded = false;
        }
        if (g_bIsUserOn){
            for (XnUserID nID = 1; nID <= maxNumUsers; nID++) {
                if (currentTrackedUsers[nID].bIsFound) {
                    cout << LOG_NAME << ": trying to kill user " << nID << endl;
                    stopTrackingUser(nID);
                }
            }
            currentTrackedUsers.clear();
            currentTrackedUserIDs.clear();
        }
        //cout << LOG_NAME << ": trying one last update of generator" << endl;
        //g_Context.WaitNoneUpdateAll(); // maybe this helps?
        cout << LOG_NAME << ": trying to unlock" << endl;
        //unlock();
        cout << LOG_NAME << ": waiting for thread to end" << endl;
        if (isThreadRunning()) waitForThread(true);
        
    }
    
    //    cout << LOG_NAME << ": trying to stop generator" << endl;
    //    g_Context.StopGeneratingAll();
    
    cout << LOG_NAME << ": releasing all nodes" << endl;
    
    stopCommon();
    
    bUseRegistration = false;
    bUseSync = false;
    bUseMirror = false;
    currentTrackedHands.clear();
    currentTrackedHandIDs.clear();
    availableGestures.clear();
    minTimeBetweenGestures = 0;
    
    if (bIsContextReady){
        cout << LOG_NAME << ": releasing context" << endl;
        g_Context.StopGeneratingAll();
        g_Context.Release();
        bIsContextReady = false;
    }
    
    instanceCount--; // ok this will probably cause problems when dynamically creating and destroying -> you'd need to do it in order!
    
    cout << LOG_NAME << ": full stopped" << endl;
}

//--------------------------------------------------------------
void ofxOpenNI::stopCommon(){
//    if (isThreadRunning()) waitForThread(true);
    if (g_bIsDepthOn){
        cout << LOG_NAME << ": releasing depth generator" << endl;
        g_Depth.StopGenerating();
        g_Depth.Release();
        l_bIsDepthRawOnOption = g_bIsDepthRawOnOption;
        l_bIsDepthOn = g_bIsDepthOn;
        g_bIsDepthRawOnOption = false;
        g_bIsDepthOn = false;
    }
        
    if (g_bIsImageOn){
        cout << LOG_NAME << ": releasing image generator" << endl;
        g_Image.StopGenerating();
        g_Image.Release();
        l_bIsImageOn = g_bIsImageOn;
        g_bIsImageOn = false;
    }
    
    if (g_bIsInfraOn){
        cout << LOG_NAME << ": releasing infra generator" << endl;
        g_Infra.StopGenerating();
        g_Infra.Release();
        l_bIsInfraOn = g_bIsInfraOn;
        g_bIsInfraOn = false;
    }
        
    if (g_bIsAudioOn){
        cout << LOG_NAME << ": releasing audio generator" << endl;
        g_Audio.StopGenerating();
        g_Audio.Release();
        g_bIsAudioOn = l_bIsAudioOn;
    }
    
    if (g_bIsUserOn){
        cout << LOG_NAME << ": releasing user generator" << endl;
        g_User.StopGenerating();
        g_User.Release();
        l_bIsUserOn = g_bIsUserOn;
        g_bIsUserOn = false;
    }
    
    if (g_bIsGestureOn){
        cout << LOG_NAME << ": releasing gesture generator" << endl;
        g_Gesture.StopGenerating();
        g_Gesture.Release();
        l_bIsGestureOn = g_bIsGestureOn;
        g_bIsGestureOn = false;
    }
    
    if (g_bIsHandsOn){
        cout << LOG_NAME << ": releasing hands generator" << endl;
        g_Hands.StopGenerating();
        g_Hands.Release();
        g_HandsFocusGesture.StopGenerating();
        g_HandsFocusGesture.Release();
        l_bIsHandsOn = g_bIsHandsOn;
        g_bIsHandsOn = false;
    }
    
    if (g_bIsPlayerOn){
        cout << LOG_NAME << ": releasing player" << endl;
        g_Player.Release();
        g_bIsPlayerOn = false;
    }
    
    if (bIsDeviceReady){
        cout << LOG_NAME << ": releasing device" << endl;
        g_Device.Release();
        bIsDeviceReady = false;
    }
//    if (bIsThreaded) startThread(true, false);
}

/**************************************************************
 *
 *      logging helper methods
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::setLogLevel(XnLogSeverity logLevel){
	XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = xnLogSetConsoleOutput(true);
	SHOW_RC(nRetVal, "Set log to console");
	nRetVal = xnLogSetSeverityFilter(logLevel);
	SHOW_RC(nRetVal, "Set log level: " + logLevel);
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);
}

//--------------------------------------------------------------
void ofxOpenNI::setLogLevel(ofLogLevel level){
    LOG_NAME = "ofxOpenNIDevice[" + ofToString(instanceID) + "]";
    logLevel = level;
    ofSetLogLevel(LOG_NAME, logLevel);
}

//--------------------------------------------------------------
void ofxOpenNI::logErrors(xn::EnumerationErrors & errors){
	for(xn::EnumerationErrors::Iterator it = errors.Begin(); it != errors.End(); ++it){
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc, 512);
        ofLogWarning(LOG_NAME) << desc << "failed:" << xnGetStatusString(it.Error());
	}	
}

/**************************************************************
 *
 *      recording ONI methods
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::startRecording(string oniFileName, XnCodecID depthFormat, XnCodecID imageFormat, XnCodecID irFormat, XnCodecID audioFormat){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    if (!g_bIsRecordOn) addGenerator(XN_NODE_TYPE_RECORDER, g_bIsRecordOn);
    if (!g_bIsRecordOn){
        ofLogError(LOG_NAME) << "Can't add the recorder!!";
        return false;
    }
    
    XnStatus nRetVal = XN_STATUS_OK;
    string ofOniFileName = ofToDataPath(oniFileName);
    nRetVal = g_Recorder.SetDestination(XN_RECORD_MEDIUM_FILE, ofOniFileName.c_str());
	SHOW_RC(nRetVal, "Set recording output file: " + ofOniFileName);
    g_bIsRecordOn = (nRetVal == XN_STATUS_OK);
    
    nRetVal = g_Recorder.AddNodeToRecording(g_Device);
    SHOW_RC(nRetVal, "Adding device generator to recording");
    g_bIsRecordOn = (nRetVal == XN_STATUS_OK);
    
    if (g_bIsDepthOn) {
        nRetVal = g_Recorder.AddNodeToRecording(g_Depth, depthFormat);
        SHOW_RC(nRetVal, "Adding depth generator to recording");
        g_bIsRecordOn = (nRetVal == XN_STATUS_OK);
    }
    
    if (g_bIsImageOn) {
        nRetVal = g_Recorder.AddNodeToRecording(g_Image, imageFormat);
        SHOW_RC(nRetVal, "Adding image generator to recording");
        g_bIsRecordOn = (nRetVal == XN_STATUS_OK);
    }
    
    if (g_bIsInfraOn) {
        nRetVal = g_Recorder.AddNodeToRecording(g_Infra, irFormat);
        SHOW_RC(nRetVal, "Adding infra generator to recording");
        g_bIsRecordOn = (nRetVal == XN_STATUS_OK);
    }
    
    if (g_bIsAudioOn) {
        nRetVal = g_Recorder.AddNodeToRecording(g_Audio, audioFormat);
        SHOW_RC(nRetVal, "Adding audio generator to recording");
        g_bIsRecordOn = (nRetVal == XN_STATUS_OK);
    }
	return g_bIsRecordOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::stopRecording(){
    bDoStop = true;
    
    return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::isRecording(){
    return g_bIsRecordOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::startPlayer(string oniFileName){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    stopCommon();
    XnStatus nRetVal = XN_STATUS_OK;
    string ofOniFileName = ofToDataPath(oniFileName);
    nRetVal = g_Context.OpenFileRecording(ofOniFileName.c_str());
    SHOW_RC(nRetVal, "Loading ONI: " + ofOniFileName);
    initCommon();
//    restartCommon();
    return g_bIsPlayerOn;
}

//--------------------------------------------------------------
//bool ofxOpenNI::stopPlayer(bool restartGenerators){
//    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
//    if  (g_bIsPlayerOn){
//        stopCommon();
//        if (restartGenerators) restartCommon();
//    }
//}

//--------------------------------------------------------------
bool ofxOpenNI::isPlaying(){
    return g_bIsPlayerOn;
}

//--------------------------------------------------------------
void ofxOpenNI::restartCommon(){
    if (l_bIsDepthOn && !g_bIsDepthOn) addDepthGenerator();
    if (l_bIsImageOn && !g_bIsImageOn) addImageGenerator();
    if (l_bIsInfraOn && !g_bIsInfraOn) addInfraGenerator();
    if (l_bIsUserOn && !g_bIsUserOn) addUserGenerator();
    if (l_bIsHandsOn && !g_bIsHandsOn) addHandsGenerator(); // TODO: store focus gestures...and reinit
    if (l_bIsGestureOn && !g_bIsGestureOn) addGestureGenerator(); // TODO: store gestures...and reinit
}

/**************************************************************
 *
 *      adding generator methods
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::addDepthGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    addGenerator(XN_NODE_TYPE_DEPTH, g_bIsDepthOn);
    if (g_bIsDepthOn) allocateDepthBuffers();
    setMirror(bUseMirror);
	return g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addImageGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    addGenerator(XN_NODE_TYPE_IMAGE, g_bIsImageOn);
    if (g_bIsImageOn) allocateImageBuffers();
    setMirror(bUseMirror);
	return g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addInfraGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    addGenerator(XN_NODE_TYPE_IR, g_bIsInfraOn);
    if (g_bIsInfraOn) allocateIRBuffers();
    setMirror(bUseMirror);
	return g_bIsInfraOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addUserGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    if (instanceID > 0) {
        // see: http://groups.google.com/group/openni-dev/browse_thread/thread/188a2ac823584117
        ofLogWarning(LOG_NAME) << "Currently it is only possible to have a user generator on one device in a single process!!";
        g_bIsUserOn = false;
        return g_bIsUserOn;   // uncomment this to see what happens
    }
    addGenerator(XN_NODE_TYPE_USER, g_bIsUserOn);
    if (g_bIsUserOn) allocateUsers();
	return g_bIsUserOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addGestureGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    addGenerator(XN_NODE_TYPE_GESTURE, g_bIsGestureOn);
    if (g_bIsGestureOn) allocateGestures();
	return g_bIsGestureOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addHandsGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    addGenerator(XN_NODE_TYPE_HANDS, g_bIsHandsOn);
    if (g_bIsHandsOn) allocateHands();
	return g_bIsHandsOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addAudioGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
	ofLogWarning(LOG_NAME) << "Not yet implimented";
    return false;
}

void ofxOpenNI::addGenerator(XnPredefinedProductionNodeType type, bool & bIsOn){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    string generatorType = getNodeTypeAsString(type);
    ofLogNotice(LOG_NAME) << "Adding generator type" << generatorType;
    XnStatus nRetVal = XN_STATUS_OK;
    if (!bIsContextReady){
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
        bIsOn = false;
        return;
	}
    if (bIsOn){
        ofLogWarning() << "Can't add" <<  generatorType << " - there is already a generator of this type and you can only have one";
        //bIsOn = false;
        return;
    }
    bIsOn = false;
    switch (type) {
		case XN_NODE_TYPE_DEPTH:
			nRetVal = g_Depth.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (!setGeneratorResolution(g_Depth, width, height, fps)) return;
            if (g_Depth.IsValid()) nRetVal = g_Depth.StartGenerating();
			break;
		case XN_NODE_TYPE_IMAGE:
			nRetVal = g_Image.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (!setGeneratorResolution(g_Image, width, height, fps)) return;
            if (g_Image.IsValid()) nRetVal = g_Image.StartGenerating();
			break;
		case XN_NODE_TYPE_AUDIO:
			nRetVal = g_Audio.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (g_Audio.IsValid()) nRetVal = g_Audio.StartGenerating();
			break;
		case XN_NODE_TYPE_IR:
			nRetVal = g_Infra.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (!setGeneratorResolution(g_Infra, width, height, fps)) return;
            if (g_Infra.IsValid()) nRetVal = g_Infra.StartGenerating();
			break;
		case XN_NODE_TYPE_USER:
			nRetVal = g_User.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (g_User.IsValid()) nRetVal = g_User.StartGenerating();
			break;
		case XN_NODE_TYPE_GESTURE:
			nRetVal = g_Gesture.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (g_Gesture.IsValid()) nRetVal = g_Gesture.StartGenerating();
			break;
		case XN_NODE_TYPE_SCENE:
			nRetVal = g_Scene.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (g_Scene.IsValid()) nRetVal = g_Scene.StartGenerating();
			break;
		case XN_NODE_TYPE_HANDS:
			nRetVal = g_Hands.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            if (g_Hands.IsValid()) nRetVal = g_Hands.StartGenerating();
            SHOW_RC(nRetVal, "Starting " + generatorType + " generator");
            nRetVal = g_HandsFocusGesture.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + getNodeTypeAsString(XN_NODE_TYPE_GESTURE) + " generator for focus gestures (needed with hands)");
            if (g_HandsFocusGesture.IsValid()) nRetVal = g_HandsFocusGesture.StartGenerating();
			break;
        case XN_NODE_TYPE_RECORDER:
			nRetVal = g_Recorder.Create(g_Context);
            SHOW_RC(nRetVal, "Creating " + generatorType + " generator");
            //if (g_Recorder.IsValid()) nRetVal = g_Recorder.StartGenerating();
			break;
		default:
			ofLogError(LOG_NAME) << "Don't know how to add this type of generator/node";
            return;
			break;
    }
    SHOW_RC(nRetVal, "Starting " + generatorType + " generator");
    bIsOn = (nRetVal == XN_STATUS_OK);
}

/**************************************************************
 *
 *      removing generator methods
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::removeDepthGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    removeGenerator(XN_NODE_TYPE_DEPTH, g_bIsDepthOn);
    return !g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeImageGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    removeGenerator(XN_NODE_TYPE_IMAGE, g_bIsImageOn);
    return !g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeInfraGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    removeGenerator(XN_NODE_TYPE_IR, g_bIsInfraOn);
    return !g_bIsInfraOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeUserGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    removeGenerator(XN_NODE_TYPE_USER, g_bIsUserOn);
    return !g_bIsUserOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeGestureGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    removeGenerator(XN_NODE_TYPE_GESTURE, g_bIsGestureOn);
    return !g_bIsGestureOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeHandsGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    removeGenerator(XN_NODE_TYPE_HANDS, g_bIsHandsOn);
    return !g_bIsHandsOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeAudioGenerator(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogWarning(LOG_NAME) << "Not yet implimented";
    return false;
}

//--------------------------------------------------------------
void ofxOpenNI::removeGenerator(XnPredefinedProductionNodeType type, bool & bIsOn){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    string generatorType = getNodeTypeAsString(type);
    ofLogNotice(LOG_NAME) << "Removing generator type" << generatorType;
    XnStatus nRetVal = XN_STATUS_OK;
    if (!bIsContextReady){
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
        bIsOn = false;
        return;
	}
    if (!bIsOn){
        ofLogWarning() << "Can't remove" <<  generatorType << " as it's not on!";
        bIsOn = false;
        return;
    }
    bIsOn = false;
    switch (type) {
		case XN_NODE_TYPE_DEPTH:
			nRetVal = g_Depth.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Depth.Release();
			break;
		case XN_NODE_TYPE_IMAGE:
            nRetVal = g_Image.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Image.Release();
			break;
		case XN_NODE_TYPE_AUDIO:
            nRetVal = g_Audio.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Audio.Release();
			break;
		case XN_NODE_TYPE_IR:
            nRetVal = g_Infra.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Infra.Release();
			break;
		case XN_NODE_TYPE_USER:
            nRetVal = g_User.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_User.Release();
			break;
		case XN_NODE_TYPE_GESTURE:
            nRetVal = g_Gesture.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Gesture.Release();
			break;
		case XN_NODE_TYPE_SCENE:
            nRetVal = g_Scene.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Scene.Release();
			break;
		case XN_NODE_TYPE_HANDS:
            nRetVal = g_Hands.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_Hands.Release();
            nRetVal = g_HandsFocusGesture.StopGenerating();
            if (nRetVal == XN_STATUS_OK) g_HandsFocusGesture.Release();
			break;
		default:
			ofLogError(LOG_NAME) << "Don't know how to remove this type of generator/node";
            return;
			break;
    }
    SHOW_RC(nRetVal, "Stopping " + generatorType + " generator");
    bIsOn = !(nRetVal == XN_STATUS_OK);
}

/**************************************************************
 *
 *      allocators for generators (pixels, textures, users)
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::allocateDepthBuffers(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating depth";
    maxDepth = g_Depth.GetDeviceMaxDepth();
    depthPixels[0].allocate(width, height, OF_IMAGE_COLOR_ALPHA);
    depthPixels[1].allocate(width, height, OF_IMAGE_COLOR_ALPHA);
    currentDepthPixels = &depthPixels[0];
    backDepthPixels = &depthPixels[1];
    if (bUseTexture) depthTexture.allocate(width, height, GL_RGBA);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateDepthRawBuffers(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating depth raw";
    maxDepth = g_Depth.GetDeviceMaxDepth();
    depthRawPixels[0].allocate(width, height, OF_PIXELS_MONO);
    depthRawPixels[1].allocate(width, height, OF_PIXELS_MONO);
    currentDepthRawPixels = &depthRawPixels[0];
    backDepthRawPixels = &depthRawPixels[1];
    return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateImageBuffers(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating image";
    imagePixels[0].allocate(width, height, OF_IMAGE_COLOR);
    imagePixels[1].allocate(width, height, OF_IMAGE_COLOR);
    currentImagePixels = &imagePixels[0];
    backImagePixels = &imagePixels[1];
    if (bUseTexture) imageTexture.allocate(width, height, GL_RGB);
    return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateIRBuffers(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating infra";
    imagePixels[0].allocate(width, height, OF_IMAGE_GRAYSCALE);
    imagePixels[1].allocate(width, height, OF_IMAGE_GRAYSCALE);
    currentImagePixels = &imagePixels[0];
    backImagePixels = &imagePixels[1];
    if (bUseTexture) imageTexture.allocate(width, height, GL_LUMINANCE);
    return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateUsers(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating users";
    XnStatus nRetVal = XN_STATUS_OK;
    bool ok = false;
    
    setMaxNumUsers(maxNumUsers); // default to 1
    
    // register user callbacks
    XnCallbackHandle User_CallbackHandler;
    XnCallbackHandle Calibration_CallbackHandler;
    
	nRetVal = g_User.RegisterUserCallbacks(UserCB_handleNewUser, UserCB_handleLostUser, this, User_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register user New/Lost callbacks");
    
	nRetVal = g_User.GetSkeletonCap().RegisterToCalibrationStart(UserCB_handleCalibrationStart, this, Calibration_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register user Calibration Start callback");
    
//    nRetVal = userGenerator.GetSkeletonCap().RegisterToCalibrationInProgress(UserCB_handleCalibrationProgress, this, Calibration_CallbackHandler);
//    BOOL_ERR_RC(nRetVal, "Register user Calibration Progress callback");
    
	nRetVal = g_User.GetSkeletonCap().RegisterToCalibrationComplete(UserCB_handleCalibrationEnd, this, Calibration_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register user Calibration End callback");
    
    // check need for calibration
    if (g_User.GetSkeletonCap().NeedPoseForCalibration()){
        ofLogNotice(LOG_NAME) << "User generator DOES require pose for calibration";
        bAutoCalibrationPossible = false;
    } else {
        ofLogNotice(LOG_NAME) << "User generator DOES NOT require pose for calibration";
        bAutoCalibrationPossible = true;
    } 
    
    // we add these even if no pose is required so that individual users can be forced to strike a pose
    if(!g_User.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
        ofLogError(LOG_NAME) << "Pose detection not supported";
    } else {
        XnCallbackHandle Pose_CallbackHandler;
        nRetVal = g_User.GetPoseDetectionCap().RegisterToPoseDetected(UserCB_handlePoseDetected, this, Pose_CallbackHandler);
        BOOL_ERR_RC(nRetVal, "Register user Pose Detected callback");
        
        nRetVal = g_User.GetSkeletonCap().GetCalibrationPose(userCalibrationPose);
        BOOL_ERR_RC(nRetVal, "Get calibration pose");
    }
    
	setSkeletonProfile(skeletonProfile);
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateGestures(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating gestures";
    XnStatus nRetVal = XN_STATUS_OK;
    lastGestureEvent.timestampMillis = 0; // used to know this is first event in CB handlers
	XnCallbackHandle Gesture_CallbackHandler;
	nRetVal = g_Gesture.RegisterGestureCallbacks(GestureCB_handleGestureRecognized, GestureCB_handleGestureProgress, this, Gesture_CallbackHandler);
    BOOL_RC(nRetVal, "Register gesture callback handlers");
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateHands(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofLogVerbose(LOG_NAME) << "Allocating hands";
    currentTrackedHands.clear();
    currentTrackedHandIDs.clear();
    ofPoint p = ofPoint(INFINITY, INFINITY, INFINITY);
    lastHandEvent = ofxOpenNIHandEvent(getDeviceID(), HAND_TRACKING_STOPPED, NULL, worldToProjective(p), p, ofGetElapsedTimeMillis());
    XnStatus nRetVal = XN_STATUS_OK;
    XnCallbackHandle Hands_CallbackHandler;
    nRetVal = g_HandsFocusGesture.RegisterGestureCallbacks(HandsCB_handleGestureRecognized, HandsCB_handleGestureProgress, this, Hands_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register hands focus gesture callback handlers");
    nRetVal = g_Hands.RegisterHandCallbacks(HandsCB_handleHandCreate, HandsCB_handleHandUpdate, HandsCB_handleHandDestroy, this, Hands_CallbackHandler);
    BOOL_RC(nRetVal, "Register hands callback handlers");
}

/**************************************************************
 *
 *      update methods (frame, user, pixels, cloudpoints)
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::threadedFunction(){
	while(isThreadRunning()){
		updateGenerators();
	}
}

//--------------------------------------------------------------
void ofxOpenNI::update(){

    if (!bIsContextReady) return;
    
	if (!bIsThreaded){
		updateGenerators();
	} else {
		lock();
	}
	
	if (bNewPixels){
		if (bUseTexture && g_bIsDepthOn){
            if (bUseBackBuffer) {
                depthTexture.loadData(*currentDepthPixels); // see note about back buffering above
            } else {
                depthTexture.loadData(*backDepthPixels);
            }
		}
		if (bUseTexture && (g_bIsImageOn || g_bIsInfraOn)){
            if (bUseBackBuffer) {
                imageTexture.loadData(*currentImagePixels);
            } else {
                imageTexture.loadData(*backImagePixels);
            }
		}
        
		for (int nID = 1; nID <= maxNumUsers; nID++) {
            ofxOpenNIUser & user = currentTrackedUsers[nID];
            if (user.bIsTracking){
                if (user.bUsePointCloud && user.bNewPointCloud){
                    swap(user.pointCloud[0], user.pointCloud[1]);
                }
                if (user.bUseMaskTexture && user.bNewPixels){
                    if (user.maskTexture.getWidth() != getWidth() || user.maskTexture.getHeight() != getHeight()){
                        ofLogVerbose(LOG_NAME) << "Allocating mask texture " << user.id;
                        user.maskTexture.allocate(getWidth(), getHeight(), GL_RGBA);
                    }
                    user.maskTexture.loadData(user.maskPixels.getPixels(), getWidth(), getHeight(), GL_RGBA);
                }
                user.bNewPointCloud = false;
                user.bNewPixels = false;
            }
            
        }
        
        bNewPixels = false;
		bNewFrame = true;

	}

	if (bIsThreaded) unlock();
}

//--------------------------------------------------------------
void ofxOpenNI::updateGenerators(){
    
	if (bIsThreaded && bUseSafeThreading) lock(); // with this here I get ~30 fps with 2 Kinects/60 fps with 1 kinect -> BUT no crash on exit!
	
    if (!bIsContextReady) return;
    
    g_Context.WaitAnyUpdateAll();
    
    //    this doesn't seem as fast/smooth, but is more 'correct':
    //    if (g_bIsUserOn) {
    //        g_Context.WaitOneUpdateAll(g_User);
    //    } else {
    //        g_Context.WaitAnyUpdateAll();
    //    }
    
	if (g_bIsDepthOn && g_Depth.IsDataNew()) g_Depth.GetMetaData(g_DepthMD);
	if (g_bIsImageOn && g_Image.IsDataNew()) g_Image.GetMetaData(g_ImageMD);
	if (g_bIsInfraOn && g_Infra.IsDataNew()) g_Infra.GetMetaData(g_InfraMD);
    
    if (bIsThreaded && !bUseSafeThreading) lock(); // with this her I get ~400-500+ fps with 2 Kinects!
    
    if (g_bIsDepthOn) updateDepthPixels();
	if (g_bIsImageOn) updateImagePixels();
	if (g_bIsInfraOn) updateIRPixels();
    if (g_bIsUserOn) updateUserTracker();
    if (g_bIsHandsOn) updateHandTracker();
    
    // NB: Below info is from my old single context setup - need to retest with this new multicontext setup!  
    // NEW SETUP for 12 frames tested avg -69.33ms latency with 2 x kinects (high ~80ms, low ~50ms)
    
    // I really don't think it's necessary to back buffer the image/ir/depth pixels
    // as I understand it the GetMetaData() call is already acting as a back buffer
    // since it is fetching the pixel data from the xn::Context which we then 'copy'
    // during our generateDepth/Image/IRPixels() methods...
    
    // my tests show that it adds between ~10 to ~15 milliseconds to capture <-> screen latency 
    // ie., the time between something occuring in the physical world, it's capture and subsequent display onscreen.
    
    // without back buffering my tests show 55 to 65ms, avg 61.5ms (consistent frame times, ie., no outliers in small samples)
    // with back buffering my tests show 70 to 80ms, avg 73.8ms (this does not include outliers ie., usually 1 in 7 frames showing 150-275ms latency!)
    
    // NB: the above tests were done with 2 Kinects...with one Kinect (and not using backbuffering) I get between 50-60ms, avg ~53ms 
    // (with some more outliers though one frame 33ms (!) andother 95ms(!))....hmmmm   
    
    if (bUseBackBuffer){
        if (g_bIsDepthOn && g_Depth.IsDataNew()){
            swap(backDepthPixels, currentDepthPixels);
            if (g_bIsDepthRawOnOption){
                swap(backDepthRawPixels, currentDepthRawPixels);
            }
        }
        if ((g_bIsImageOn && g_Image.IsDataNew()) || (g_bIsInfraOn && g_Infra.IsDataNew())){
            swap(backImagePixels, currentImagePixels);
        }
    }
	
	bNewPixels = true;
    
    if (bDoStop) {
        //if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
        //g_Context.WaitAnyUpdateAll();
        if (!g_bIsRecordOn){
            ofLogError(LOG_NAME) << "There is a problem with the recorder or you haven't called startRecording()";
            //return false;
        }else{
            XnStatus nRetVal = XN_STATUS_OK;
            nRetVal = g_Recorder.RemoveNodeFromRecording(g_Device);
            SHOW_RC(nRetVal, "Removing device generator from recording");
            g_bIsRecordOn = (nRetVal != XN_STATUS_OK);
            
            if (g_bIsDepthOn) {
                nRetVal = g_Recorder.RemoveNodeFromRecording(g_Depth);
                SHOW_RC(nRetVal, "Removing depth generator from recording");
                g_bIsRecordOn = (nRetVal != XN_STATUS_OK);
            }
            
            if (g_bIsImageOn) {
                nRetVal = g_Recorder.RemoveNodeFromRecording(g_Image);
                SHOW_RC(nRetVal, "Removing image generator from recording");
                g_bIsRecordOn = (nRetVal != XN_STATUS_OK);
            }
            
            if (g_bIsInfraOn) {
                nRetVal = g_Recorder.RemoveNodeFromRecording(g_Infra);
                SHOW_RC(nRetVal, "Removing infra generator from recording");
                g_bIsRecordOn = (nRetVal != XN_STATUS_OK);
            }
            
            if (g_bIsAudioOn) {
                nRetVal = g_Recorder.RemoveNodeFromRecording(g_Audio);
                SHOW_RC(nRetVal, "Removing audio generator from recording");
                g_bIsRecordOn = (nRetVal != XN_STATUS_OK);
            }
            g_Recorder.Release();
            g_bIsRecordOn = false;
        }
        
        bDoStop = false;
    }
    
	if (bIsThreaded) unlock();
}

/**************************************************************
 *
 *      update pixels and textures (depth, image, infra)
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::updateDepthPixels(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
	// get the pixels
	const XnDepthPixel* depth = g_DepthMD.Data();
	
	if (g_DepthMD.FrameID() == 0) return;
	
	// copy raw values
	if (g_bIsDepthRawOnOption){
		backDepthRawPixels->setFromPixels(depth, g_DepthMD.XRes(), g_DepthMD.YRes(), OF_IMAGE_COLOR);
	}
	
	// copy depth into texture-map
	for (XnUInt16 y = g_DepthMD.YOffset(); y < g_DepthMD.YRes() + g_DepthMD.YOffset(); y++){
		unsigned char * texture = backDepthPixels->getPixels() + y * g_DepthMD.XRes() * 4 + g_DepthMD.XOffset() * 4;
		for (XnUInt16 x = 0; x < g_DepthMD.XRes(); x++, depth++, texture += 4){
			
			ofColor depthColor;
			getDepthColor(depthColoring, *depth, depthColor, maxDepth);
			
			texture[0] = depthColor.r;
			texture[1] = depthColor.g;
			texture[2] = depthColor.b;
			
			if (*depth == 0)
				texture[3] = 0;
			else
				texture[3] = depthColor.a;
		}
	}
}

//--------------------------------------------------------------
void ofxOpenNI::updateImagePixels(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
	const XnUInt8* pImage = g_ImageMD.Data();
	backImagePixels->setFromPixels(pImage, g_ImageMD.XRes(), g_ImageMD.YRes(), OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofxOpenNI::updateIRPixels(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
	const XnIRPixel* pImage = g_InfraMD.Data();
    unsigned char * ir_pixels = new unsigned char[g_InfraMD.XRes() * g_InfraMD.YRes()];
	for (int i = 0; i < g_InfraMD.XRes() * g_InfraMD.YRes(); i++){
		ir_pixels[i] = pImage[i]/4;
	}
	backImagePixels->setFromPixels(ir_pixels, g_InfraMD.XRes(), g_InfraMD.YRes(), OF_IMAGE_GRAYSCALE);
    delete ir_pixels;
}

/**************************************************************
 *
 *      update hand/s tracker
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::updateHandTracker(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    map<XnUserID, ofxOpenNIHand>::iterator it;
    int index = 0;
    currentTrackedHandIDs.clear();
    for (it = currentTrackedHands.begin(); it != currentTrackedHands.end(); it++, index++){
        ofxOpenNIHand & hand = it->second;
        if (hand.isTracking()) {
            currentTrackedHandIDs.push_back(hand.getID());
        }else{
            currentTrackedHands.erase(it);
        }
    }
}

/**************************************************************
 *
 *      update skeletons, user masks and point clouds
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::updateUserTracker(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
	vector<XnUserID> userIDs(maxNumUsers);
    XnUInt16 xnMaxNumUsers = maxNumUsers;
	g_User.GetUsers(&userIDs[0], xnMaxNumUsers);
	set<XnUserID> trackedUserIDs;
    
	for (int i = 0; i < maxNumUsers; ++i) {
		if (g_User.GetSkeletonCap().IsTracking(userIDs[i])) {
			ofxOpenNIUser & user = currentTrackedUsers[userIDs[i]];
			user.id = userIDs[i];
			XnPoint3D center;
			g_User.GetCoM(userIDs[i], center);
			user.center = toOf(center);
            bool lastbIsSkeleton = user.bIsSkeleton;
            user.bIsSkeleton = false;
			for (int j=0; j<ofxOpenNIUser::NumLimbs; j++){
				XnSkeletonJointPosition a,b;
				g_User.GetSkeletonCap().GetSkeletonJointPosition(user.id, user.limbs[j].start_joint, a);
				g_User.GetSkeletonCap().GetSkeletonJointPosition(user.id, user.limbs[j].end_joint, b);
				g_User.GetSkeletonCap().GetSkeletonJointOrientation(user.id,user.limbs[j].start_joint, user.limbs[j].orientation);
				if (a.fConfidence < user.limbDetectionConfidence || b.fConfidence < user.limbDetectionConfidence){
					user.limbs[j].found = false;
					continue;
				}
				user.limbs[j].found = true;
				user.limbs[j].begin = worldToProjective(a.position);
				user.limbs[j].end = worldToProjective(b.position);
				user.limbs[j].worldBegin = toOf(a.position);
				user.limbs[j].worldEnd = toOf(b.position);
                user.bIsSkeleton = true;
			}
            
            if (user.bUsePointCloud || user.bUseMaskPixels) {
                xn::SceneMetaData smd;
                if (g_User.GetUserPixels(user.id, smd) == XN_STATUS_OK) {
                    user.userPixels = (unsigned short*)smd.Data();
                }
            }

			if (user.bUsePointCloud) updatePointClouds(user);
			if (user.bUseMaskPixels || user.bUseMaskTexture) updateUserPixels(user);
            
			trackedUserIDs.insert(user.id);
            
            if (user.bIsSkeleton != lastbIsSkeleton){
                ofLogNotice(LOG_NAME) << "Skeleton" << (string)(user.bIsSkeleton ? "found" : "lost") << "for user" << user.id;
                ofxOpenNIUserEvent event = ofxOpenNIUserEvent(getDeviceID(), (user.bIsSkeleton ? USER_SKELETON_FOUND : USER_SKELETON_LOST), user.id, ofGetElapsedTimeMillis());
                ofNotifyEvent(userEvent, event, this);
            }
		}
	}
    currentTrackedUserIDs.assign(trackedUserIDs.begin(), trackedUserIDs.end());
}

//--------------------------------------------------------------
void ofxOpenNI::updatePointClouds(ofxOpenNIUser & user){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
	const XnRGB24Pixel*	pColor;
	const XnDepthPixel*	pDepth = g_DepthMD.Data();
    
	if (g_bIsImageOn) {
		pColor = g_ImageMD.RGB24Data();
	}
    
	int step = user.getPointCloudResolution();
	int nIndex = 0;
    
	user.pointCloud[0].getVertices().clear();
	user.pointCloud[0].getColors().clear();
	user.pointCloud[0].setMode(OF_PRIMITIVE_POINTS);
    
	for (int nY = 0; nY < getHeight(); nY += step) {
		for (int nX = 0; nX < getWidth(); nX += step) {
            nIndex = nY * getWidth() + nX;
			if (user.userPixels[nIndex] == user.id) {
				user.pointCloud[0].addVertex(ofPoint(nX, nY, pDepth[nIndex]));
				if(g_bIsImageOn){
					user.pointCloud[0].addColor(ofColor(pColor[nIndex].nRed, pColor[nIndex].nGreen, pColor[nIndex].nBlue));
				}else{
					user.pointCloud[0].addColor(ofFloatColor(1,1,1));
				}
			}
		}
	}
    user.bNewPointCloud = true;
}

//--------------------------------------------------------------
void ofxOpenNI::updateUserPixels(ofxOpenNIUser & user){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    if (user.maskPixels.getWidth() != getWidth() || user.maskPixels.getHeight() != getHeight()){
        user.maskPixels.allocate(getWidth(), getHeight(), OF_IMAGE_COLOR_ALPHA);
    }
	
    int nIndex = 0;
    for (int nY = 0; nY < getHeight(); nY++) {
		for (int nX = 0; nX < getWidth(); nX++) {
            nIndex = nY * getWidth() + nX;
            if (user.userPixels[nIndex] == user.id) {
                user.maskPixels[nIndex * 4 + 0] = 255;
                user.maskPixels[nIndex * 4 + 1] = 255;
                user.maskPixels[nIndex * 4 + 2] = 255;
                user.maskPixels[nIndex * 4 + 3] = 0;
            } else {
                user.maskPixels[nIndex * 4 + 0] = 0;
                user.maskPixels[nIndex * 4 + 1] = 0;
                user.maskPixels[nIndex * 4 + 2] = 0;
                user.maskPixels[nIndex * 4 + 3] = 255;
            }
        }
    }
    user.bNewPixels = true;
}

/**************************************************************
 *
 *      getters/setters: user properties
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::setUserSmoothing(float smooth){
	if (smooth > 0.0f && smooth < 1.0f) {
		userSmoothFactor = smooth;
		if (g_bIsUserOn) {
			g_User.GetSkeletonCap().SetSmoothing(smooth);
		}
	}
}

//--------------------------------------------------------------
float ofxOpenNI::getUserSmoothing(){
	return userSmoothFactor;
}

bool ofxOpenNI::setSkeletonProfile(XnSkeletonProfile profile){
    skeletonProfile = profile;
    if (g_bIsUserOn){
        XnStatus nRetVal = XN_STATUS_OK;
        nRetVal = g_User.GetSkeletonCap().SetSkeletonProfile(skeletonProfile);
        BOOL_RC(nRetVal, "Set skeleton profile");
    }
}

XnSkeletonProfile ofxOpenNI::getSkeletonProfile(){
    return skeletonProfile;
}

//--------------------------------------------------------------
void ofxOpenNI::resetUserTracking(XnUserID nID, bool forceImmediateRestart){
    stopTrackingUser(nID);
    if (forceImmediateRestart) startTrackingUser(nID);
}

//--------------------------------------------------------------
bool ofxOpenNI::getAutoUserCalibrationPossible(){
	return bAutoCalibrationPossible;
}

//--------------------------------------------------------------
int	ofxOpenNI::getNumTrackedUsers(){
    return currentTrackedUserIDs.size();
}

//--------------------------------------------------------------
ofxOpenNIUser& ofxOpenNI::getTrackedUser(int index){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    return currentTrackedUsers[currentTrackedUserIDs[index]];
}

//--------------------------------------------------------------
ofxOpenNIUser& ofxOpenNI::getUser(XnUserID nID){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofxOpenNIUser & user = baseUser;
    map<XnUserID,ofxOpenNIUser>::iterator it = currentTrackedUsers.find(nID);
    if (it != currentTrackedUsers.end()){
        user = it->second;
    }else{
        if (nID == 0) {
            ofLogError(LOG_NAME) << "You have requested a user ID of 0 - perhaps you wanted to use getTrackedUser()"
            << "OR you need to iterate using something like: for (int i = 1; i <= openNIDevices[0].getMaxNumUsers(); i++)"
            << "Returning a reference to the baseUserClass user (it doesn't do anything!!!)!";
        }else{
            ofLogError() << "User ID not found. Probably you need to setMaxNumUsers to a higher value!"
            << "Returning a reference to the baseUserClass user (it doesn't do anything!!!)!";
        }
        
        user.id = 0;
    }
    return user;
}

//--------------------------------------------------------------
void ofxOpenNI::setMaxNumUsers(int numUsers){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    maxNumUsers = numUsers;
    int oldMaxUsers = currentTrackedUsers.size();
    
    if (maxNumUsers < oldMaxUsers) {
        for (XnUserID nID = maxNumUsers + 1; nID <= oldMaxUsers; ++nID){
            map<XnUserID, ofxOpenNIUser>::iterator it = currentTrackedUsers.find(nID);
            currentTrackedUsers.erase(it);
        }
    } else {
        for (XnUserID nID = oldMaxUsers + 1; nID <= maxNumUsers; ++nID){
            //ofxOpenNIUser user;
            baseUser.id = nID;
            currentTrackedUsers.insert(pair<XnUserID, ofxOpenNIUser>(nID, baseUser));
        }
    }
    
    ofLogVerbose(LOG_NAME) << "Resized tracked user map from" << oldMaxUsers << "to" << currentTrackedUsers.size();
}

//--------------------------------------------------------------
int	ofxOpenNI::getMaxNumUsers(){
    return maxNumUsers; // currentTrackedUsers.size()
}

//--------------------------------------------------------------
void ofxOpenNI::setUseMaskTextureAllUsers(bool b){
    for (XnUserID nID = 1; nID <= maxNumUsers; ++nID){
        currentTrackedUsers[nID].setUseMaskTexture(b);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::setUseMaskPixelsAllUsers(bool b){
    for (XnUserID nID = 1; nID <= maxNumUsers; ++nID){
        currentTrackedUsers[nID].setUseMaskPixels(b);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::setUsePointCloudsAllUsers(bool b){
    for (XnUserID nID = 1; nID <= maxNumUsers; ++nID){
        currentTrackedUsers[nID].setUsePointCloud(b);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::setPointCloudDrawSizeAllUsers(int size){
    for (XnUserID nID = 1; nID <= maxNumUsers; ++nID){
        currentTrackedUsers[nID].setPointCloudDrawSize(size);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::setPointCloudResolutionAllUsers(int resolution){
    for (XnUserID nID = 1; nID <= maxNumUsers; ++nID){
        currentTrackedUsers[nID].setPointCloudResolution(resolution);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::setBaseUserClass(ofxOpenNIUser & user){
    baseUser = user;
    for (XnUserID nID = 1; nID <= maxNumUsers; ++nID){
        currentTrackedUsers[nID] = user;
    }
}

/**************************************************************
 *
 *      getters/setters: gesture properties
 *
 *************************************************************/

//--------------------------------------------------------------
vector<string> & ofxOpenNI::getAvailableGestures(){
    if (!g_bIsGestureOn && !g_bIsHandsOn){
        ofLogError(LOG_NAME) << "Can't get available gestures there isn't a gesture generator - use addGestureGenerator() or addHandsGenerator() first";
        availableGestures.clear();
        return availableGestures;
    }
    if (availableGestures.size() == 0){
        XnStatus nRetVal = XN_STATUS_OK;
        XnUInt16 nGestures = 0;
        if (g_bIsGestureOn) nGestures = g_Gesture.GetNumberOfAvailableGestures();
        if (g_bIsHandsOn) nGestures = g_HandsFocusGesture.GetNumberOfAvailableGestures();
        if (nGestures == 0){
            ofLogError(LOG_NAME) << "No gestures available";
            return availableGestures;
        }
        availableGestures.resize(nGestures);
        XnChar* astrGestures[nGestures];
        for (int i = 0; i < nGestures; i++){
            astrGestures[i] = new XnChar[255];
        }
        if (g_bIsGestureOn) nRetVal = g_Gesture.EnumerateGestures(*astrGestures, nGestures);
        if (g_bIsHandsOn) nRetVal = g_HandsFocusGesture.EnumerateGestures(*astrGestures, nGestures);
        SHOW_RC(nRetVal, "Enumerating gestures");
        if (nRetVal != XN_STATUS_OK) {
            ofLogError(LOG_NAME) << "Could not enumerate gestures";
            return availableGestures;
        }
        ofLogNotice(LOG_NAME) << nGestures << "gestures available";
        for (int i = 0; i < nGestures; i++){
            ofLogVerbose(LOG_NAME) << i << "-" << astrGestures[i] << "- use addGesture(\"" << astrGestures[i] << "\") to recieve events for this gesture";
            availableGestures[i] = (string)astrGestures[i];
            delete [] astrGestures[i];
        }
    }
    return availableGestures;
}

bool ofxOpenNI::isGestureAvailable(string niteGestureName){
    if (!g_bIsGestureOn && !g_bIsHandsOn){
        ofLogError(LOG_NAME) << "Gesture not available because there isn't a gesture generator - use addGestureGenerator() or addHandsGenerator() first";
        return false;
    }
    if (availableGestures.size() == 0){
        if(getAvailableGestures().size() == 0){
            return false;
        }
    }
    bool available = false;
    for (int i = 0; i < availableGestures.size(); i++){
        if (availableGestures[i] == niteGestureName) {
            available = true;
            break;
        }
    }
    return available;
}

//--------------------------------------------------------------
bool ofxOpenNI::addAllGestures(){
    if (!g_bIsGestureOn){
        ofLogError(LOG_NAME) << "Can't add all gestures as there isn't a gesture generator - use addGestureGenerator() first";
        return false;
    }
    if (getAvailableGestures().size() == 0){
        ofLogError(LOG_NAME) << "Can't add all gestures as there are none available";
        return false;
    }
    for (int i = 0; i < availableGestures.size(); i++) {
        addGesture(availableGestures[i]);
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::removeAllGestures(){
    if (!g_bIsGestureOn){
        ofLogError(LOG_NAME) << "Can't remove all gestures as there isn't a gesture generator - use addGestureGenerator() first";
        return false;
    }
    if (getAvailableGestures().size() == 0){
        ofLogError(LOG_NAME) << "Can't remove all gestures as there are none available";
        return false;
    }
    for (int i = 0; i < availableGestures.size(); i++) {
        removeGesture(availableGestures[i]);
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::addGesture(string niteGestureName, ofPoint LeftBottomNear, ofPoint RightTopFar){
    if (!g_bIsGestureOn){
        ofLogError(LOG_NAME) << "Can't add gesture as there isn't a gesture generator - use addGestureGenerator() first";
        return false;
    }
    if (!isGestureAvailable(niteGestureName)){
        ofLogError(LOG_NAME) << "Can't add gesture as this is an unkown type -> use getAvailableGestures to get a vector<string> of gesture names";
        return false;
    }
    // AddGesture takes a XnBoundingBox3D area:
    // typedef struct XnBoundingBox3D
    // {
    //     XnPoint3D LeftBottomNear;
    //     XnPoint3D RightTopFar;
    // } XnBoundingBox3D;
    // This is in real world co-ordinates ie., mm's to the left/bottom/near
    // and right/top/far of the sensor see: http://groups.google.com/group/openni-dev/browse_thread/thread/90cbf7414979ad14/cc91592be432ba4c
    // Lior Cohen:
    // "LeftBottomNear.X = (-300); 
    // LeftBottomNear.Y = (-300); 
    // LeftBottomNear.Z = (1000); 
    // RightTopFar.X = (300); 
    // RightTopFar.Y = (300); 
    // RightTopFar.Z = (2000); 
    // These two points will create a XnBoundingBox3D with 600x600x1000mm dimensions and hanging in mid air from the sensor with a center point at (0,0,1500) mm."
    
    // TODO: add id's to area so we can fire events specifically for these bounding areas
    XnBoundingBox3D * boundingBox3D = NULL;
    if (LeftBottomNear != NULL && RightTopFar != NULL) {
        ofLogWarning(LOG_NAME) << "LeftBottomNear and RightTopFar should be in world co-ordinates ie., they are in mm's and left.x/bottom.y from the centre of the sensor is negative, whilst right.x/top.y is positive; depth.z is always positive starting at 0 to maxDepth (10000) mm";
        boundingBox3D = new XnBoundingBox3D;
        boundingBox3D->LeftBottomNear = toXn(LeftBottomNear);
        boundingBox3D->RightTopFar = toXn(RightTopFar);
        // TODO: implement multimap to track active gesture hot spots??
    }
    
    ofLogNotice(LOG_NAME) << "Adding NITE gesture" << niteGestureName;
	
    XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = g_Gesture.AddGesture(niteGestureName.c_str(), boundingBox3D);
	BOOL_RC(nRetVal, "Adding simple (NITE) gesture " + niteGestureName);
}

//--------------------------------------------------------------
bool ofxOpenNI::removeGesture(string niteGestureName){
    if (!g_bIsGestureOn){
        ofLogError(LOG_NAME) << "Can't remove gesture as there isn't a gesture generator - use addGestureGenerator() first";
        return false;
    }
    if (!isGestureAvailable(niteGestureName)){
        ofLogError(LOG_NAME) << "Can't remove gesture as this is an unkown type -> use getAvailableGestures to get a vector<string> of gesture names";
        return false;
    }
    ofLogNotice(LOG_NAME) << "Removing NITE gesture" << niteGestureName;
	XnStatus nRetVal = XN_STATUS_OK;	
	nRetVal = g_Gesture.RemoveGesture(niteGestureName.c_str());
	BOOL_RC(nRetVal, "Removing simple (NITE) gesture " + niteGestureName);
}

//--------------------------------------------------------------
void ofxOpenNI::setMinTimeBetweenGestures(int millis){
    minTimeBetweenGestures = millis;
}

//--------------------------------------------------------------
int ofxOpenNI::getMinTimeBetweenGestures(){
    return minTimeBetweenGestures;
}

/**************************************************************
 *
 *      getters/setters: hands properties
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::addAllHandFocusGestures(){
    if (!g_bIsHandsOn){
        ofLogError(LOG_NAME) << "Can't add all focus gestures as there isn't a gesture generator - use addHandsGenerator() first";
        return false;
    }
    if (getAvailableGestures().size() == 0){
        ofLogError(LOG_NAME) << "Can't add all focus gestures as there are none available";
        return false;
    }
    for (int i = 0; i < availableGestures.size(); i++) {
        addGesture(availableGestures[i]);
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::removeAllHandFocusGestures(){
    if (!g_bIsHandsOn){
        ofLogError(LOG_NAME) << "Can't remove all focus gestures as there isn't a gesture generator - use addHandsGenerator() first";
        return false;
    }
    if (getAvailableGestures().size() == 0){
        ofLogError(LOG_NAME) << "Can't remove all gestures as there are none available";
        return false;
    }
    for (int i = 0; i < availableGestures.size(); i++) {
        removeGesture(availableGestures[i]);
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::addHandFocusGesture(string niteGestureName, ofPoint LeftBottomNear, ofPoint RightTopFar){
    if (!g_bIsHandsOn){
        ofLogError(LOG_NAME) << "Can't add gesture as there isn't a hand focus gesture generator - use addHandsGenerator() first";
        return false;
    }
    if (!isGestureAvailable(niteGestureName)){
        ofLogError(LOG_NAME) << "Can't add gesture as this is an unkown type -> use getAvailableGestures to get a vector<string> of gesture names";
        return false;
    }
    
    // TODO: add id's to area so we can fire events specifically for these bounding areas
    XnBoundingBox3D * boundingBox3D = NULL;
    if (LeftBottomNear != NULL && RightTopFar != NULL) {
        ofLogWarning(LOG_NAME) << "LeftBottomNear and RightTopFar should be in world co-ordinates ie., they are in mm's and left.x/bottom.y from the centre of the sensor is negative, whilst right.x/top.y is positive; depth.z is always positive starting at 0 to maxDepth (10000) mm";
        boundingBox3D = new XnBoundingBox3D;
        boundingBox3D->LeftBottomNear = toXn(LeftBottomNear);
        boundingBox3D->RightTopFar = toXn(RightTopFar);
        // TODO: implement multimap to track active gesture hot spots??
    }
    
    ofLogNotice(LOG_NAME) << "Adding NITE hand focus gesture" << niteGestureName;
	
    XnStatus nRetVal = XN_STATUS_OK;
	nRetVal = g_HandsFocusGesture.AddGesture(niteGestureName.c_str(), boundingBox3D);
	BOOL_RC(nRetVal, "Adding simple (NITE) gesture " + niteGestureName);
}

//--------------------------------------------------------------
bool ofxOpenNI::removeHandFocusGesture(string niteGestureName){
    if (!g_bIsHandsOn){
        ofLogError(LOG_NAME) << "Can't remove gesture as there isn't a gesture generator - use addGestureGenerator() first";
        return false;
    }
    if (!isGestureAvailable(niteGestureName)){
        ofLogError(LOG_NAME) << "Can't remove gesture as this is an unkown type -> use getAvailableGestures to get a vector<string> of gesture names";
        return false;
    }
    ofLogNotice(LOG_NAME) << "Removing NITE hand focus gesture" << niteGestureName;
	XnStatus nRetVal = XN_STATUS_OK;	
	nRetVal = g_HandsFocusGesture.RemoveGesture(niteGestureName.c_str());
	BOOL_RC(nRetVal, "Removing simple (NITE) gesture " + niteGestureName);
}

//--------------------------------------------------------------
int	ofxOpenNI::getNumTrackedHands(){
    return currentTrackedHandIDs.size();
}

//--------------------------------------------------------------
ofxOpenNIHand& ofxOpenNI::getTrackedHand(int index){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    return currentTrackedHands[currentTrackedHandIDs[index]];
}

//--------------------------------------------------------------
ofxOpenNIHand& ofxOpenNI::getHand(XnUserID nID){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    ofxOpenNIHand & hand = baseHand;
    map<XnUserID, ofxOpenNIHand>::iterator it = currentTrackedHands.find(nID);
    if (it != currentTrackedHands.end()){
        hand = it->second;
    }else{
        if (nID == 0) {
            ofLogError(LOG_NAME) << "You have requested a hand ID of 0 - perhaps you wanted to use getTrackedHand()" 
            << "Returning a reference to the baseUserClass user (it doesn't do anything!!!)!";
        }else{
            ofLogError() << "User ID not found. Probably you need to setMaxNumUsers to a higher value!" 
            << "Returning a reference to the baseUserClass user (it doesn't do anything!!!)!";
        }
        hand.id = 0;
    }
    return hand;
}

//--------------------------------------------------------------
void ofxOpenNI::setMaxNumHands(int numHands){
    maxNumHands = numHands;
}

//--------------------------------------------------------------
int ofxOpenNI::getMaxNumHands(){
    return maxNumHands;
}

//--------------------------------------------------------------
void ofxOpenNI::setMinTimeBetweenHands(int millis){
    minTimeBetweenHands = millis;
}

//--------------------------------------------------------------
int ofxOpenNI::getMinTimeBetweenHands(){
    return minTimeBetweenHands;
}

//--------------------------------------------------------------
void ofxOpenNI::setMinDistanceBetweenHands(int worldDistance){
    minDistanceBetweenHands = worldDistance;
}

//--------------------------------------------------------------
int ofxOpenNI::getMinDistanceBetweenHands(){
    return minDistanceBetweenHands;
}

//--------------------------------------------------------------
void ofxOpenNI::setBaseHandClass(ofxOpenNIHand & hand){
    baseHand = hand;
}

/**************************************************************
 *
 *      getters/setters: rgb to depth calibration properties
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::toggleRegister(){
	setRegister(!bUseRegistration);
}

//--------------------------------------------------------------
void ofxOpenNI::setRegister(bool b){
    XnStatus nRetVal = XN_STATUS_OK;
    if (!g_bIsDepthOn){
        ofLogVerbose(LOG_NAME) << "Depth generator is not on";
        return;
    }
    if (!g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)){
        ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
        bUseRegistration = false;
    }
    bUseRegistration = false;
    if (b){
        if (!g_Image.IsValid()){
            ofLogError(LOG_NAME) << "No Image generator found: cannot register image to depth";
            bUseRegistration = false;
            return;
        }
        // Register view point to image map
        nRetVal = g_Depth.GetAlternativeViewPointCap().SetViewPoint(g_Image);
        SHOW_RC(nRetVal, "Register viewpoint depth to RGB");
        bUseRegistration = (nRetVal == XN_STATUS_OK);
        return;
    }else{
        nRetVal = g_Depth.GetAlternativeViewPointCap().ResetViewPoint();
        SHOW_RC(nRetVal, "Unregister viewpoint depth to RGB");
        bUseRegistration = !(nRetVal == XN_STATUS_OK);
        return;
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::getRegister(){
	return bUseRegistration;
}

//--------------------------------------------------------------
void ofxOpenNI::toggleSync(){
	setSync(!bUseSync);
}

//--------------------------------------------------------------
void ofxOpenNI::setSync(bool b){
    XnStatus nRetVal = XN_STATUS_OK;
    if (!g_bIsDepthOn){
        ofLogVerbose(LOG_NAME) << "Depth generator is not on";
        return;
    }
    if (!g_Depth.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC)){
        ofLogVerbose(LOG_NAME) << "Depth does not support frame sync";
        bUseSync = false;
        return;
    }
    if (b){
        bUseSync = true;
        // Try to start frame sync any nodes that are already on
        if (g_bIsDepthOn && g_bIsImageOn){
            nRetVal = g_Depth.GetFrameSyncCap().CanFrameSyncWith(g_Image);
            if (nRetVal != XN_STATUS_OK) {
                ofLogError(LOG_NAME) << "Depth cannot sync with image"; //  - doesn't seem to work with my Kinects
                bUseSync = false;
                return;
            }
            nRetVal = g_Depth.GetFrameSyncCap().FrameSyncWith(g_Image);
            SHOW_RC(nRetVal, "Frame sync depth and image generators");
            bUseSync = (nRetVal == XN_STATUS_OK);
        }
        if (g_bIsDepthOn && g_bIsInfraOn){
            nRetVal = g_Depth.GetFrameSyncCap().CanFrameSyncWith(g_Infra);
            if (nRetVal != XN_STATUS_OK) {
                ofLogError(LOG_NAME) << "Depth cannot sync with infra";
                bUseSync = false;
                return;
            }
            nRetVal = g_Depth.GetFrameSyncCap().FrameSyncWith(g_Infra);
            SHOW_RC(nRetVal, "Frame sync depth and infra generators"); //  - doesn't seem to work with my Kinects
            bUseSync = (nRetVal == XN_STATUS_OK);
        }
    }
    if (!b){
        bUseSync = false;
        // Stop frame sync for any nodes that are already on
        if (g_bIsDepthOn && g_bIsImageOn){
            nRetVal = g_Depth.GetFrameSyncCap().StopFrameSyncWith(g_Image);
            SHOW_RC(nRetVal, "Stop frame sync depth and image generators");
            bUseSync = (nRetVal == XN_STATUS_OK);
        }
        if (g_bIsDepthOn && g_bIsInfraOn){
            nRetVal = g_Depth.GetFrameSyncCap().StopFrameSyncWith(g_Infra);
            SHOW_RC(nRetVal, "Stop frame sync depth and infra generators");
            bUseSync = (nRetVal == XN_STATUS_OK);
        }
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::getSync(){
	return bUseSync;
}

//--------------------------------------------------------------
void ofxOpenNI::toggleMirror(){
	setMirror(!bUseMirror);
}

//--------------------------------------------------------------
void ofxOpenNI::setMirror(bool b){
    XnStatus nRetVal = XN_STATUS_OK;
    bool isMirrorPossible = true;
    if (g_bIsDepthOn && !g_Depth.IsCapabilitySupported(XN_CAPABILITY_MIRROR)){
        ofLogVerbose(LOG_NAME) << "Mirror depth capability not supported";
        isMirrorPossible = false;
    }
    if (g_bIsImageOn && !g_Image.IsCapabilitySupported(XN_CAPABILITY_MIRROR)){
        ofLogVerbose(LOG_NAME) << "Mirror image capability not supported";
        isMirrorPossible = false;
    }
    if (g_bIsInfraOn && !g_Infra.IsCapabilitySupported(XN_CAPABILITY_MIRROR)){
        ofLogVerbose(LOG_NAME) << "Mirror infra capability not supported";
        isMirrorPossible = false;
    }
    if (!isMirrorPossible) {
        ofLogError(LOG_NAME) << "Either there are no generators or none of them support mirroring";
        bUseMirror = false;
        return;
    }
    bUseMirror = b;
    if (g_bIsDepthOn && g_Depth.GetMirrorCap().IsMirrored() != bUseMirror){
        nRetVal = g_Depth.GetMirrorCap().SetMirror((XnBool)bUseMirror);
        SHOW_RC(nRetVal, "Set mirror depth " + (string)(bUseMirror ? "ON" : "OFF"));
        if (nRetVal != XN_STATUS_OK) {
            bUseMirror = !bUseMirror;
            return;
        }
    }
    if (g_bIsImageOn && g_Image.GetMirrorCap().IsMirrored() != bUseMirror){
        nRetVal = g_Image.GetMirrorCap().SetMirror((XnBool)bUseMirror);
        SHOW_RC(nRetVal, "Set mirror image " + (string)(bUseMirror ? "ON" : "OFF"));
        if (nRetVal != XN_STATUS_OK) {
            bUseMirror = !bUseMirror;
            return;
        }
    }
    if (g_bIsInfraOn && g_Infra.GetMirrorCap().IsMirrored() != bUseMirror){
        nRetVal = g_Infra.GetMirrorCap().SetMirror((XnBool)bUseMirror);
        SHOW_RC(nRetVal, "Set mirror infra " + (string)(bUseMirror ? "ON" : "OFF"));
        if (nRetVal != XN_STATUS_OK) {
            bUseMirror = !bUseMirror;
            return;
        }
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::getMirror(){
	return bUseMirror;
}

/**************************************************************
 *
 *      getters/setters: threading
 *
 *************************************************************/

#if defined (TARGET_OSX) && defined (USE_SIGNALS_HACK)
// this is a nasty and unadvisable hack to silence crashes on exit
// the correct way to deal with this is to use setSafeThreading(true)
// BUT then application FPS goes from 400+ to 30-60fps and latency increases...so I'm using this for now :-)
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
void handleSignal(int err){
    cout << "CAUGHT SIGNAL ERROR: " << err << " either SIGSEGV or SIGBUS" << endl;
    cout << "This handler is a NASTY hack due to a known error in ofxOpenNI on (at least) Mac OSX with setSafeThreading(false)" << endl;
    cout << "this just silences the Mac CrashReporter in a live environment (it will NOT fire if you have GDB running)" << endl;
    cout << "...it's possible some other bit of your code has sent you here by mistake..." << endl;
    exit(0);  
}
#endif

//--------------------------------------------------------------
void ofxOpenNI::setSafeThreading(bool b){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock();
    bUseSafeThreading = b;
    if (b) return;
#if defined (TARGET_OSX) && defined (USE_SIGNALS_HACK)
    ofLogWarning(LOG_NAME) << "Using a NASTY hack to silence SIGNAL errors on exit - read the comments at line ~1712 of ofxOpenNI.cpp";
    if (signal(SIGSEGV, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGSEGV!";
    if (signal(SIGBUS, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGBUS!";
    //if (signal(SIGINT, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGABRT!";
    //if (signal(SIGABRT, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGABRT!";
    //if (signal(SIGILL, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGILL!";
    //if (signal(SIGFPE, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGFPE!";
    //if (signal(SIGPIPE, handleSignal) == SIG_ERR) ofLogWarning(LOG_NAME) << "Cannot handle SIGPIPE!";
#endif
}

//--------------------------------------------------------------
bool ofxOpenNI::getSafeThreading(){
    return bUseSafeThreading;
}

/**************************************************************
 *
 *      getters/setters: pixel and texture properties/modes
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::setUseBackBuffer(bool b){
	bUseBackBuffer = b;
}

//--------------------------------------------------------------
bool ofxOpenNI::getUseBackBuffer(){
	return bUseBackBuffer;
}

//--------------------------------------------------------------
void ofxOpenNI::setDepthColoring(DepthColoring coloring){
	depthColoring = coloring;
}

//--------------------------------------------------------------
bool ofxOpenNI::isNewFrame(){
	return bNewFrame;
}

/**************************************************************
 *
 *      getters: pixels and textures (direct access)
 *
 *************************************************************/

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getDepthPixels(){
	if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	if (bUseBackBuffer){
        return *currentDepthPixels;
    }else{
        return *backDepthPixels;
    }
}

//--------------------------------------------------------------
ofShortPixels& ofxOpenNI::getDepthRawPixels(){
	if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	if (!g_bIsDepthRawOnOption){
		ofLogWarning(LOG_NAME) << "g_bIsDepthRawOnOption was disabled, enabling raw pixels";
		g_bIsDepthRawOnOption = true;
	}
    if (bUseBackBuffer){
        return *currentDepthRawPixels;
    }else{
        return *backDepthRawPixels;
    }
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getImagePixels(){
	if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
    if (bUseBackBuffer){
        return *currentImagePixels;
    }else{
        return *backImagePixels;
    }
}

//--------------------------------------------------------------
ofTexture& ofxOpenNI::getDepthTextureReference(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	return depthTexture;
}

//--------------------------------------------------------------
ofTexture& ofxOpenNI::getimageTextureReference(){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	return imageTexture;
}

/**************************************************************
 *
 *      getters/setters: width, height and resolution
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::setResolution(int w, int h, int f){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
    
    ofLogWarning(LOG_NAME) << "This is still experimental...for now not implimented";
    return false; // uncomment to give it a try
    
    XnResolution m_Res = xnResolutionGetFromXYRes(w, h);
    string resolutionType = xnResolutionGetName(m_Res);
    ofLogNotice(LOG_NAME) << "Requested resolution of" << w << " x " << h << "is" << resolutionType << "at" << f << "fps";
    
    int oW = width; int oH = height; int oF = fps;
    width = w; height = h; fps = f;
    
    bool ok = !bIsContextReady;
    if (g_bIsDepthOn) {
        ok = allocateDepthBuffers();
        if(!ok) {
            width = oW; height = oH; fps = oF;
            allocateDepthBuffers();
        }
    }
    
    if (g_bIsImageOn) {
        ok = allocateImageBuffers();
        if(!ok) {
            width = oW; height = oH; fps = oF;
            allocateImageBuffers();
        }
    }
    
    if (g_bIsInfraOn) {
        ok = allocateIRBuffers();
        if(!ok) {
            width = oW; height = oH; fps = oF;
            allocateIRBuffers();
        }
    }
    
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::setGeneratorResolution(xn::MapGenerator & generator, int w, int h, int f){
    if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
    XnMapOutputMode mapMode;
    XnStatus nRetVal = XN_STATUS_OK;
    mapMode.nXRes = w; mapMode.nYRes = h; mapMode.nFPS  = f;
    if (generator.IsValid()){
        nRetVal = generator.SetMapOutputMode(mapMode);
        BOOL_RC(nRetVal, "Setting " + (string)generator.GetName() + " resolution: " + ofToString(mapMode.nXRes) + " x " + ofToString(mapMode.nYRes) + " at " + ofToString(mapMode.nFPS) + "fps");
    } else {
        ofLogError(LOG_NAME) << "setGeneratorResolution() called on invalid generator!";
        return false;
    }
}

//--------------------------------------------------------------
float ofxOpenNI::getWidth(){
	if (g_bIsDepthOn){
		return g_DepthMD.XRes();
	}else if (g_bIsImageOn){
		return g_ImageMD.XRes();
	}else if (g_bIsInfraOn){
		return g_InfraMD.XRes();
	} else {
		ofLogWarning(LOG_NAME) << "getWidth() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

//--------------------------------------------------------------
float ofxOpenNI::getHeight(){
	if (g_bIsDepthOn){
		return g_DepthMD.YRes();
	}else if (g_bIsImageOn){
		return g_ImageMD.YRes();
	}else if (g_bIsInfraOn){
		return g_InfraMD.YRes();
	} else {
		ofLogWarning(LOG_NAME) << "getHeight() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

/**************************************************************
 *
 *      getters: generator presence
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::isDepthOn(){
    return g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::isImageOn(){
    return g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::isInfraOn(){
    return g_bIsInfraOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::isUserOn(){
    return g_bIsUserOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::isGestureOn(){
    return g_bIsGestureOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::isHandsOn(){
    return g_bIsHandsOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::isAudioOn(){
    return g_bIsAudioOn;
}

/**************************************************************
 *
 *      getters: context and device presence
 *
 *************************************************************/

//--------------------------------------------------------------
bool ofxOpenNI::isContextReady(){
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNI::getNumDevices(){
    if (!bIsContextReady) setup();
	return numDevices;
}

//--------------------------------------------------------------
int ofxOpenNI::getDeviceID(){
	return instanceID;
}

/**************************************************************
 *
 *      getters: generator reference (xn::type direct access)
 *
 *************************************************************/

//--------------------------------------------------------------
xn::Context& ofxOpenNI::getContext(){
	return g_Context;
}

//--------------------------------------------------------------
xn::Device& ofxOpenNI::getDevice(){
	return g_Device;
}

//--------------------------------------------------------------
xn::DepthGenerator& ofxOpenNI::getDepthGenerator(){
	return g_Depth;
}

//--------------------------------------------------------------
xn::ImageGenerator& ofxOpenNI::getImageGenerator(){
	return g_Image;
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNI::getIRGenerator(){
	return g_Infra;
}

//--------------------------------------------------------------
xn::UserGenerator& ofxOpenNI::getUserGenerator(){
	return g_User;
}

//--------------------------------------------------------------
xn::GestureGenerator& ofxOpenNI::getGestureGenerator(){
	return g_Gesture;
}

//--------------------------------------------------------------
xn::HandsGenerator& ofxOpenNI::getHandsGenerator(){
	return g_Hands;
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNI::getAudioGenerator(){
	return g_Audio;
}

//--------------------------------------------------------------
xn::DepthMetaData& ofxOpenNI::getDepthMetaData(){
	return g_DepthMD;
}

//--------------------------------------------------------------
xn::ImageMetaData& ofxOpenNI::getImageMetaData(){
	return g_ImageMD;
}

//--------------------------------------------------------------
xn::IRMetaData& ofxOpenNI::getIRMetaData(){
	return g_InfraMD;
}

//--------------------------------------------------------------
xn::AudioMetaData& ofxOpenNI::getAudioMetaData(){
	return g_AudioMD;
}

/**************************************************************
 *
 *      callbacks: user generator callback handlers
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::startTrackingUser(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    if (nID > getMaxNumUsers()){
        ofLogNotice(LOG_NAME) << "Start tracking cancelled for user" << nID << "since maxNumUsers is" << maxNumUsers;
        stopTrackingUser(nID);
        return;
    } else {
        ofLogNotice(LOG_NAME) << "Start tracking user" << nID;
    }
	nRetVal = g_User.GetSkeletonCap().StartTracking(nID);
    CHECK_ERR_RC(nRetVal, "Get skeleton capability - start tracking");
    if (nRetVal == XN_STATUS_OK){
        currentTrackedUsers[nID].bIsFound = true;
        currentTrackedUsers[nID].bIsTracking = true;
        currentTrackedUsers[nID].bIsCalibrating = false;
        ofxOpenNIUserEvent event = ofxOpenNIUserEvent(getDeviceID(), USER_TRACKING_STARTED, nID, ofGetElapsedTimeMillis());
        ofNotifyEvent(userEvent, event, this);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::stopTrackingUser(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    if (g_User.GetSkeletonCap().IsCalibrating(nID)){// || g_User.GetSkeletonCap().IsCalibrated(nID)){
        ofLogNotice(LOG_NAME) << "Calibration stopped for user" << nID;
        nRetVal = g_User.GetSkeletonCap().AbortCalibration(nID);
        CHECK_ERR_RC(nRetVal, "Get skeleton capability - stop calibrating");
    }
    if (g_User.GetSkeletonCap().IsTracking(nID)){
        ofLogNotice(LOG_NAME) << "Skeleton tracking stopped for user" << nID;
        nRetVal = g_User.GetSkeletonCap().Reset(nID);
        CHECK_ERR_RC(nRetVal, "Get skeleton capability - stop tracking");
    }
    if (nID > getMaxNumUsers()) return;
    ofLogNotice(LOG_NAME) << "Stop tracking user" << nID;
    currentTrackedUsers[nID].bIsFound = false;
    currentTrackedUsers[nID].bIsTracking = false;
    currentTrackedUsers[nID].bIsSkeleton = false;
    currentTrackedUsers[nID].bIsCalibrating = false;
    ofxOpenNIUserEvent event = ofxOpenNIUserEvent(getDeviceID(), USER_TRACKING_STOPPED, nID, ofGetElapsedTimeMillis());
    ofNotifyEvent(userEvent, event, this);
}

//--------------------------------------------------------------
void ofxOpenNI::requestCalibration(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    if (nID > maxNumUsers){
        ofLogVerbose(LOG_NAME) << "Calibration requested cancelled for user" << nID << "since maxNumUsers is" << maxNumUsers;
        return;
    } else {
        ofLogNotice(LOG_NAME) << "Calibration requested for user" << nID;
    }
	nRetVal = g_User.GetSkeletonCap().RequestCalibration(nID, TRUE);
    CHECK_ERR_RC(nRetVal, "Get skeleton capability - request calibration");
    if (nRetVal == XN_STATUS_OK){
        currentTrackedUsers[nID].bIsFound = true;
        currentTrackedUsers[nID].bIsCalibrating = true;
        ofxOpenNIUserEvent event = ofxOpenNIUserEvent(getDeviceID(), USER_CALIBRATION_STARTED, nID, ofGetElapsedTimeMillis());
        ofNotifyEvent(userEvent, event, this);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::startPoseDetection(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    if (nID > maxNumUsers){
        ofLogVerbose(LOG_NAME) << "Pose detection cancelled for user" << nID << "since maxNumUsers is" << maxNumUsers;
        stopPoseDetection(nID);
        return;
    } else {
        ofLogNotice(LOG_NAME) << "Start pose detection for user" << nID;
    }
	nRetVal = g_User.GetPoseDetectionCap().StartPoseDetection(userCalibrationPose, nID);
    SHOW_RC(nRetVal, "Get pose detection capability - start");
    if (nRetVal == XN_STATUS_OK){
        currentTrackedUsers[nID].bIsFound = true;
        currentTrackedUsers[nID].bIsCalibrating = true;
        ofxOpenNIUserEvent event = ofxOpenNIUserEvent(getDeviceID(), USER_CALIBRATION_STARTED, nID, ofGetElapsedTimeMillis());
        ofNotifyEvent(userEvent, event, this);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::stopPoseDetection(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    ofLogNotice(LOG_NAME) << "Stop pose detection for user" << nID;
	nRetVal = g_User.GetPoseDetectionCap().StopPoseDetection(nID);
    CHECK_ERR_RC(nRetVal, "Get pose detection capability - stop");
    if (nID > maxNumUsers) return;
    if (nRetVal == XN_STATUS_OK){
        currentTrackedUsers[nID].bIsCalibrating = false;
        ofxOpenNIUserEvent event = ofxOpenNIUserEvent(getDeviceID(), USER_CALIBRATION_STOPPED, nID, ofGetElapsedTimeMillis());
        ofNotifyEvent(userEvent, event, this);
    }
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCB_handleNewUser(xn::UserGenerator& userGenerator, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(openNI->LOG_NAME) << "(CB) New User" << nID;
	if(openNI->getAutoUserCalibrationPossible()) {
		openNI->requestCalibration(nID);
	} else {
        openNI->startPoseDetection(nID);
	}
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCB_handleLostUser(xn::UserGenerator& userGenerator, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(openNI->LOG_NAME) << "(CB) Lost user" << nID;
	openNI->stopTrackingUser(nID);
    
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCB_handlePoseDetected(xn::PoseDetectionCapability& poseDetectionCapability, const XnChar* strPose, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(openNI->LOG_NAME) << "(CB) Pose" << strPose << "detected for user" << nID;
	openNI->requestCalibration(nID);
	openNI->stopPoseDetection(nID);
}


//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCB_handleCalibrationStart(xn::SkeletonCapability& skeletonCapability, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(openNI->LOG_NAME) << "(CB) Calibration start for user" << nID;
}

////--------------------------------------------------------------
//void XN_CALLBACK_TYPE ofxOpenNI::UserCB_handleCalibrationProgress(xn::SkeletonCapability& skeletonCapability, XnUserID nID, XnCalibrationStatus calibrationStatus, void* pCookie){
//    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
//    ofLogVerbose(openNI->LOG_NAME) << "(CB) Calibration progress for user" << nID << calibrationStatus << getCalibrationStatusAsString(calibrationStatus);
//    //openNI->handleCalibrationProgress(nID, calibrationStatus);
//}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCB_handleCalibrationEnd(xn::SkeletonCapability& skeletonCapability, XnUserID nID, XnCalibrationStatus calibrationStatus, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(openNI->LOG_NAME) << "(CB) Calibration end for user..." << nID;
	if(calibrationStatus == XN_CALIBRATION_STATUS_OK) {
		ofLogVerbose(openNI->LOG_NAME) << "...success" << nID;
		openNI->startTrackingUser(nID);
	} else {
        ofLogVerbose(openNI->LOG_NAME) << "...fail" << nID;
		if(openNI->getAutoUserCalibrationPossible() && openNI->getUser(nID).bUseAutoCalibration) {
			openNI->requestCalibration(nID);
		} else {
            openNI->startPoseDetection(nID);
		}
	}
}

/**************************************************************
 *
 *      callbacks: gesture generator callback handlers
 *
 *************************************************************/

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::GestureCB_handleGestureRecognized(xn::GestureGenerator& gestureGenerator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(openNI->LOG_NAME)  << "(CB) Gesture Recognized: ID position (x y z) [" 
                                    << (int)pIDPosition->X << (int)pIDPosition->Y 
                                    << (int)pIDPosition->Z << "] end position (x y z) [" 
                                    << (int)pEndPosition->X << (int)pEndPosition->Y << (int)pEndPosition->Z << "]";
    
    ofxOpenNIGestureEvent & lastGestureEvent = openNI->lastGestureEvent;
    // Filter by a minimum time between firing gestures
	if (lastGestureEvent.timestampMillis == 0 || ofGetElapsedTimeMillis() > lastGestureEvent.timestampMillis + openNI->minTimeBetweenGestures){
        ofPoint p = ofPoint(pIDPosition->X, pIDPosition->Y, pIDPosition->Z);
        lastGestureEvent = ofxOpenNIGestureEvent(openNI->getDeviceID(), strGesture, GESTURE_RECOGNIZED, 1.0f, openNI->worldToProjective(p), p, ofGetElapsedTimeMillis());
		ofNotifyEvent(openNI->gestureEvent, lastGestureEvent);
	} else {
		ofLogVerbose(openNI->LOG_NAME) << "Gesture FILTERED by time:" << ofGetElapsedTimeMillis() << (lastGestureEvent.timestampMillis + openNI->minTimeBetweenGestures);
	}

}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::GestureCB_handleGestureProgress(xn::GestureGenerator& gestureGenerator, const XnChar* strGesture, const XnPoint3D* pIDPosition, XnFloat fProgress, void* pCookie){
    //ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	//ofLogVerbose(openNI->LOG_NAME) << "(CB) Gesture Progress";
    //TODO: add useProgress and add events here? or just leave it out?
}

/**************************************************************
 *
 *      callbacks: hands generator callback handlers
 *
 *************************************************************/

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::HandsCB_handleGestureRecognized(xn::GestureGenerator& gestureGenerator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
    
    if (openNI->currentTrackedHands.size() >= openNI->getMaxNumHands()){
        //ofLogVerbose(openNI->LOG_NAME) << "Not tracking because nID >= maxNumHands" << openNI->currentTrackedHands.size() << openNI->getMaxNumHands();
        return;
    }
    
    ofxOpenNIHandEvent & lastHandEvent = openNI->lastHandEvent;
    
    if (ofGetElapsedTimeMillis() < lastHandEvent.timestampMillis + openNI->minTimeBetweenHands){
        //ofLogVerbose(openNI->LOG_NAME) << "Not tracking because time between hand events is < minTimeBetweenHands";
        return;
    }
    
    ofPoint p = ofPoint(pEndPosition->Z, pEndPosition->Y, pEndPosition->Z);
    
    float distanceToHand = sqrt(pow( lastHandEvent.worldPosition.x - p.x, 2 ) +
                                pow( lastHandEvent.worldPosition.y - p.y, 2 ) +
                                pow( lastHandEvent.worldPosition.z - p.z, 2 ) );
    
    // if the hand is within the min distance then don't track it
    if (distanceToHand < openNI->minDistanceBetweenHands){
        //ofLogVerbose(openNI->LOG_NAME) << "Not tracking because distance between hands is < minDistanceBetweenHands" << distanceToHand;
        return;
    }
    
    
	ofLogVerbose(openNI->LOG_NAME)  << "(CB) Hands Focus Gesture Recognized: ID position (x y z) [" 
                                    << (int)pIDPosition->X << (int)pIDPosition->Y 
                                    << (int)pIDPosition->Z << "] end position (x y z) [" 
                                    << (int)pEndPosition->X << (int)pEndPosition->Y << (int)pEndPosition->Z << "]";

    openNI->g_Hands.StartTracking(*pEndPosition);
    lastHandEvent = ofxOpenNIHandEvent(openNI->getDeviceID(), HAND_FOCUS_GESTURE_RECOGNIZED, NULL, openNI->worldToProjective(p), p, ofGetElapsedTimeMillis());
    ofNotifyEvent(openNI->handEvent, lastHandEvent);
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::HandsCB_handleGestureProgress(xn::GestureGenerator& gestureGenerator, const XnChar* strGesture, const XnPoint3D* pIDPosition, XnFloat fProgress, void* pCookie){
    // nothing
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::HandsCB_handleHandCreate(xn::HandsGenerator& handsGenerator, XnUserID nID, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
    if (openNI->currentTrackedHands.size() + 1 <= openNI->getMaxNumHands()) {
        ofLogVerbose(openNI->LOG_NAME) << "(CB) Hands Create: OK" << nID << openNI->currentTrackedHands.size() + 1 << openNI->getMaxNumHands();
        openNI->currentTrackedHands[nID] = openNI->baseHand;// force creation of hand in map
        ofxOpenNIHand & hand = openNI->currentTrackedHands[nID];
        hand.id = nID;
        hand.bIsTracking = true;
        ofPoint p = ofPoint(pPosition->X, pPosition->Y, pPosition->Z);
        hand.position = openNI->worldToProjective(p);
        hand.worldPosition = p;
        ofxOpenNIHandEvent handEvent = ofxOpenNIHandEvent(openNI->getDeviceID(), HAND_TRACKING_STARTED, nID, hand.position, hand.worldPosition, ofGetElapsedTimeMillis());
        ofNotifyEvent(openNI->handEvent, handEvent);
    }else{
        ofLogVerbose(openNI->LOG_NAME) << "(CB) Hands Create: FAIL" << nID << openNI->currentTrackedHands.size() + 1 << openNI->getMaxNumHands();
    }
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::HandsCB_handleHandUpdate(xn::HandsGenerator& handsGenerator, XnUserID nID, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	//ofLogVerbose(openNI->LOG_NAME) << "(CB) Hands Update" << nID;
    map<XnUserID, ofxOpenNIHand>::iterator it = openNI->currentTrackedHands.find(nID);
    if (it != openNI->currentTrackedHands.end()) {
        ofxOpenNIHand & hand = it->second;
        hand.bIsTracking = true;
        ofPoint p = ofPoint(pPosition->X, pPosition->Y, pPosition->Z);
        hand.position = openNI->worldToProjective(p);
        hand.worldPosition = p;
        ofxOpenNIHandEvent handEvent = ofxOpenNIHandEvent(openNI->getDeviceID(), HAND_TRACKING_UPDATED, nID, hand.position, hand.worldPosition, ofGetElapsedTimeMillis());
        ofNotifyEvent(openNI->handEvent, handEvent);
    }
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::HandsCB_handleHandDestroy(xn::HandsGenerator& handsGenerator, XnUserID nID, XnFloat fTime, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
    map<XnUserID, ofxOpenNIHand>::iterator it = openNI->currentTrackedHands.find(nID);
    if (it != openNI->currentTrackedHands.end()) {
        ofxOpenNIHand & hand = it->second;
        hand.bIsTracking = false;
        ofxOpenNIHandEvent handEvent = ofxOpenNIHandEvent(openNI->getDeviceID(), HAND_TRACKING_STOPPED, nID, hand.position, hand.worldPosition, ofGetElapsedTimeMillis());
        ofNotifyEvent(openNI->handEvent, handEvent);
        //openNI->currentTrackedHands.erase(it);
        ofLogVerbose(openNI->LOG_NAME) << "(CB) Hands Destroy" << nID << openNI->currentTrackedHands.size();
    }else{
        ofLogError(openNI->LOG_NAME) << "(CB) Hands Destroy called on non-existant hand:" << nID;
    }
}

/**************************************************************
 *
 *      drawing: debug draw
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::drawDebug(){
	if (bIsContextReady) drawDebug(0.0f, 0.0f, -1.0f, -1.0f);
}

//--------------------------------------------------------------
void ofxOpenNI::drawDebug(float x, float y){
	if (bIsContextReady) drawDebug(x, y, -1.0f, -1.0f);
}

//--------------------------------------------------------------
void ofxOpenNI::drawDebug(float x, float y, float w, float h){
	if (!bIsContextReady) return;
    
    int generatorCount = g_bIsDepthOn + g_bIsImageOn + g_bIsInfraOn;
    float fullWidth = getWidth() * generatorCount;
    float fullHeight = getHeight();
    
    if (w == -1.0f && h == -1.0f){
        w = fullWidth;
        h = fullHeight;
    }
    
    ofPushStyle();
    
    ofPushMatrix();
    ofTranslate(x, y);
    ofScale(w / (getWidth() * generatorCount), h / getHeight());
    
    //ofPushMatrix();
    if (g_bIsDepthOn) drawDepth();
    if (g_bIsHandsOn) drawHands();
    if (g_bIsUserOn) drawSkeletons();
    ofTranslate(getWidth(), 0.0f);
    if (g_bIsImageOn || g_bIsInfraOn) drawImage();
    if (g_bIsUserOn){
        if (g_bIsImageOn || g_bIsInfraOn) ofTranslate(-getWidth(), 0.0f);
        for (int nID = 1; nID <= maxNumUsers; nID++) {
            ofxOpenNIUser & user = getUser(nID);
            ofSetColor(255, 255, 0);
            ofDrawBitmapString(user.getDebugInfo(), 8, getHeight() - (30*maxNumUsers) + (nID-1) * 30);
        }
    }
    //ofPopMatrix();
    ofPopMatrix();
    ofPopStyle();
    
}

/**************************************************************
 *
 *      drawing: depth draw
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(){
	if (bUseTexture && bIsContextReady) drawDepth(0.0f, 0.0f, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(float x, float y){
	if (bUseTexture && bIsContextReady) drawDepth(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(float x, float y, float w, float h){
	if (bUseTexture && bIsContextReady) depthTexture.draw(x, y, w, h);
}

/**************************************************************
 *
 *      drawing: image draw
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::drawImage(){
	if (bUseTexture && bIsContextReady) drawImage(0.0f, 0.0f, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage(float x, float y){
	if (bUseTexture && bIsContextReady) drawImage(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage(float x, float y, float w, float h){
	if (bUseTexture && bIsContextReady) imageTexture.draw(x, y, w, h);
}

/**************************************************************
 *
 *      drawing: skeleton draw
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::drawSkeletons(){
    if (bIsContextReady) drawSkeletons(0.0f, 0.0f, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawSkeletons(float x, float y){
	if (bIsContextReady) drawSkeletons(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawSkeletons(float x, float y, float w, float h){
	if (!bIsContextReady) return;
    for(int i = 0;  i < getNumTrackedUsers(); ++i){
        drawSkeleton(x, y, w, h, i);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::drawSkeleton(int nID){
	drawSkeleton(0.0f, 0.0f, getWidth(), getHeight(), nID);
}

//--------------------------------------------------------------
void ofxOpenNI::drawSkeleton(float x, float y, int nID){
	drawSkeleton(x, y, getWidth(), getHeight(), nID);
}

//--------------------------------------------------------------
void ofxOpenNI::drawSkeleton(float x, float y, float w, float h, int nID){
	if(nID - 1 > getNumTrackedUsers()) return;
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(x, y);
    ofScale(w/getWidth(), h/getHeight(), 1.0f);
	currentTrackedUsers[currentTrackedUserIDs[nID]].drawSkeleton();
    ofPopMatrix();
    ofPopStyle();
}

/**************************************************************
 *
 *      drawing: skeleton draw
 *
 *************************************************************/

//--------------------------------------------------------------
void ofxOpenNI::drawHands(){
    drawHands(0, 0, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawHands(float x, float y){
    drawHands(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawHands(float x, float y, float w, float h){
    for (int i = 0; i < getNumTrackedHands(); i++) {
        drawHand(i);
    }
}

//--------------------------------------------------------------
void ofxOpenNI::drawHand(int index){
    drawHand(0, 0, getWidth(), getHeight(), index);
}

//--------------------------------------------------------------
void ofxOpenNI::drawHand(float x, float y, int index){
    drawHand(x, y, getWidth(), getHeight(), index);
}

//--------------------------------------------------------------
void ofxOpenNI::drawHand(float x, float y, float w, float, int index){
    if (index > getNumTrackedHands()) return;
    ofPushStyle();
    ofPushMatrix();
    ofFill();
    ofSetColor(255, 255, 0);
    ofxOpenNIHand & hand = getTrackedHand(index);
    ofCircle(hand.position.x, hand.position.y, 10);
    ofPopMatrix();
    ofPopStyle();
}

/**************************************************************
 *
 *      pixel helper methods
 *
 *************************************************************/

//--------------------------------------------------------------
ofPoint ofxOpenNI::worldToProjective(const ofPoint& p){
	XnVector3D world = toXn(p);
	return worldToProjective(world);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::worldToProjective(const XnVector3D& p){
	XnVector3D proj;
	g_Depth.ConvertRealWorldToProjective(1,&p,&proj);
	return toOf(proj);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::projectiveToWorld(const ofPoint& p){
	XnVector3D proj = toXn(p);
	return projectiveToWorld(proj);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::projectiveToWorld(const XnVector3D& p){
	XnVector3D world;
	g_Depth.ConvertProjectiveToRealWorld(1,&p,&world);
	return toOf(world);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::cameraToWorld(const ofVec2f& c){
	vector<ofVec2f> vc(1, c);
	vector<ofVec3f> vw(1);
	cameraToWorld(vc, vw);
	return vw[0];
}

//--------------------------------------------------------------
void ofxOpenNI::cameraToWorld(const vector<ofVec2f>& c, vector<ofVec3f>& w){
	const int nPoints = c.size();
	w.resize(nPoints);
	if (!g_bIsDepthRawOnOption){
		ofLogError(LOG_NAME) << "ofxOpenNI::cameraToWorld - cannot perform this function if g_bIsDepthRawOnOption is false. You can enabled g_bIsDepthRawOnOption by calling getDepthRawPixels(..).";
		return;
	}
	
	vector<XnPoint3D> projective(nPoints);
	//XnPoint3D *out = &projective[0];
	
	//lock();
	const XnDepthPixel* d = currentDepthRawPixels->getPixels();
	unsigned int pixel;
	for (int i = 0; i < nPoints; ++i){
		pixel = (int)c[i].x + (int)c[i].y * g_DepthMD.XRes();
		if (pixel >= g_DepthMD.XRes() * g_DepthMD.YRes())
			continue;
		
		projective[i].X = c[i].x;
		projective[i].Y = c[i].y;
		projective[i].Z = float(d[pixel]) / 1000.0f;
	}
	//unlock();
	
	g_Depth.ConvertProjectiveToRealWorld(nPoints,&projective[0], (XnPoint3D*)&w[0]);
}