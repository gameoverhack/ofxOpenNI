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
static string CALLBACK_LOG_NAME = "ofxOpenNIUserCB";
//static ofMutex ofxOpenNIMutex;
static bool shutdown = false;
//--------------------------------------------------------------
ofxOpenNI::ofxOpenNI(){
	instanceCount++;
	instanceID = instanceCount; // TODO: this should be replaced/combined with a listDevices and setDeviceID methods
	
    LOG_NAME = "ofxOpenNIDevice[" + ofToString(instanceID) + "]";
    
	bIsThreaded = false;
	
	g_bIsDepthOn = false;
	g_bIsDepthRawOnOption = false;
	g_bIsImageOn = false;
	g_bIsIROn = false;
    g_bIsUserOn = false;
	g_bIsAudioOn = false;
	g_bIsPlayerOn = false;
	
	depthColoring = COLORING_RAINBOW;
	
    bIsContextReady = false;
    bUseBackBuffer = false;
    bNeedsPose = true;
	bUseTexture = true;
	bNewPixels = false;
	bNewFrame = false;
	
    width = XN_VGA_X_RES;
    height = XN_VGA_Y_RES;
    fps = 30;
    
	CreateRainbowPallet();
}

//--------------------------------------------------------------
ofxOpenNI::~ofxOpenNI(){
    if (bIsThreaded) {
        shutdown = true;
    } else {
        stop();
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(bool threaded){
	
    XnStatus nRetVal = XN_STATUS_OK;
	bIsThreaded = threaded;
    
	if (!initContext()){
        ofLogError(LOG_NAME) << "Context could not be intitilised";
        return false;
	}
    
    if (!initDevice()){
        ofLogError(LOG_NAME) << "Device could not be intitilised";
        return false;
	}
    
    if (numDevices < instanceID) {
        ofLogError(LOG_NAME) << "No Devices available for this instance [" << instanceID << "] of ofxOpenNI - have you got a device connected?!?!";
        g_Context.Release();
        bIsContextReady = false;
        return false;
    } else {
        ofLogNotice(LOG_NAME) << "Starting device for instance [" << instanceID << "] of ofxOpenNI";
        if (bIsThreaded) startThread(true, false);
        return true;
	}
	
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(string xmlFilePath, bool threaded){
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
void ofxOpenNI::stop(){

    if (!bIsContextReady) return;

    if (bIsThreaded) {
        if (shutdown) {
            stopThread();
        } else {
            //shutdown = true;
            waitForThread(true);
            //return;
        }
        
    }

    bIsThreaded = false;
    bIsContextReady = false;
    
    g_Context.StopGeneratingAll();

    instanceCount--; // ok this will probably cause problems when dynamically creating and destroying -> you'd need to do it in order!

    g_Depth.Release();
    g_Image.Release();
    g_IR.Release();
    g_User.Release();
    g_Audio.Release();
    g_Player.Release();
    g_Context.Release();
    
    ofLogVerbose(LOG_NAME) << "Stopped ofxOpenNI instance" << instanceID;
}

//--------------------------------------------------------------
bool ofxOpenNI::initContext(){
    if (bIsContextReady) return true;
    XnStatus nRetVal = XN_STATUS_OK;
    ofToDataPath(""); // of007 hack to make sure ofSetRootPath is done!
    nRetVal = g_Context.Init();
    SHOW_RC(nRetVal, "OpenNI Context initilized");
    bIsContextReady = (nRetVal == XN_STATUS_OK);
    if (bIsContextReady) addLicence("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
    return bIsContextReady;
}

//--------------------------------------------------------------
bool ofxOpenNI::initDevice(){
    if (initContext()) {
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
        
        nRetVal = g_Context.CreateProductionTree(nInfo, g_Device);
        SHOW_RC(nRetVal, "Creating production tree for device " + ofToString(instanceID));
        return (nRetVal == XN_STATUS_OK);
    } else {
        return false;
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::addLicence(string sVendor, string sKey){
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
    //xnPrintRegisteredLicenses();
    return ok;
}

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
void ofxOpenNI::setLogLevel(ofLogLevel logLevel){
    LOG_NAME = "ofxOpenNIDevice[" + ofToString(instanceID) + "]";
    ofSetLogLevel(LOG_NAME, logLevel);
}

//--------------------------------------------------------------
void ofxOpenNI::logErrors(xn::EnumerationErrors & errors){
	for(xn::EnumerationErrors::Iterator it = errors.Begin(); it != errors.End(); ++it){
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc, 512);
		ofLog(OF_LOG_ERROR, "%s failed: %s\n", desc, xnGetStatusString(it.Error()));
	}	
}

//--------------------------------------------------------------
bool ofxOpenNI::addDepthGenerator(){
	XnStatus nRetVal = XN_STATUS_OK;
    g_bIsDepthOn = false;
	ofLogNotice(LOG_NAME) << "Adding depth generator...";
	if (!bIsContextReady){
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
	}
    nRetVal = g_Depth.Create(g_Context);
    SHOW_RC(nRetVal, "Creating depth generator");
    g_bIsDepthOn = (nRetVal == XN_STATUS_OK);
    if (!g_bIsDepthOn){
        ofLogError(LOG_NAME) << "Could not add depth generator node to context";
    }
	if (g_Depth.IsValid()){
        allocateDepthBuffers();
		nRetVal = g_Depth.StartGenerating();
		SHOW_RC(nRetVal, "Starting depth generator");
        g_bIsDepthOn = (nRetVal == XN_STATUS_OK);
	} else {
		ofLogError(LOG_NAME) << "Depth generator is invalid!";
	}
	return g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addImageGenerator(){
	XnStatus nRetVal = XN_STATUS_OK;
    g_bIsImageOn = false;
	ofLogNotice(LOG_NAME) << "Adding image generator...";
	if (!bIsContextReady){
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
	}
    nRetVal = g_Image.Create(g_Context);
    SHOW_RC(nRetVal, "Creating image generator");
    g_bIsImageOn = (nRetVal == XN_STATUS_OK);
    if (!g_bIsImageOn){
        ofLogError(LOG_NAME) << "Could not add image generator node to context";
    }
	if (g_Image.IsValid()){
        allocateImageBuffers();
		nRetVal = g_Image.StartGenerating();
		SHOW_RC(nRetVal, "Starting image generator");
        g_bIsImageOn = (nRetVal == XN_STATUS_OK);
	} else {
		ofLogError(LOG_NAME) << "Image generator is invalid!";
	}
	return g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addInfraGenerator(){
	XnStatus nRetVal = XN_STATUS_OK;
    g_bIsIROn = false;
	ofLogNotice(LOG_NAME) << "Adding ir generator...";
	if (!bIsContextReady){
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
	}
    nRetVal = g_IR.Create(g_Context);
    SHOW_RC(nRetVal, "Creating IR generator");
    g_bIsIROn = (nRetVal == XN_STATUS_OK);
    if (!g_bIsIROn){
        ofLogError(LOG_NAME) << "Could not add IR generator node to context";
    }
	if (g_IR.IsValid()){
        allocateIRBuffers();
		nRetVal = g_IR.StartGenerating();
		SHOW_RC(nRetVal, "Starting IR generator");
        g_bIsIROn = (nRetVal == XN_STATUS_OK);
	} else {
		ofLogError(LOG_NAME) << "IR generator is invalid!";
	}
	return g_bIsIROn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addUserGenerator(){
	
    XnStatus nRetVal = XN_STATUS_OK;
    g_bIsUserOn = false;
    ofLogNotice(LOG_NAME) << "Adding user generator...";
    
    if (instanceID > 0) {
        ofLogWarning(LOG_NAME) << "Currently it seems only possible to have a user generator on one device in a single process!!";
        // some people say it could be done like thus: http://openni-discussions.979934.n3.nabble.com/OpenNI-dev-Skeleton-tracking-with-multiple-kinects-not-solved-with-new-OpenNI-td2832613.html ... but itdidn't work for me .... 
        // ok it's not possible yet according to: http://groups.google.com/group/openni-dev/browse_thread/thread/188a2ac823584117
        return false;   // uncomment this to see what happens
    }
    
    if (!bIsContextReady){
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
	}
    nRetVal = g_User.Create(g_Context);
    SHOW_RC(nRetVal, "Creating user generator");
    g_bIsUserOn = (nRetVal == XN_STATUS_OK);
    if (!g_bIsUserOn){
        ofLogError(LOG_NAME) << "Could not add user generator node to context";
    }
	if (g_User.IsValid()){
        allocateUsers();
		nRetVal = g_User.StartGenerating();
		SHOW_RC(nRetVal, "Starting user generator");
        g_bIsUserOn = (nRetVal == XN_STATUS_OK);
	} else {
		ofLogError(LOG_NAME) << "User generator is invalid!";
	}
	return g_bIsUserOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addAudioGenerator(){
	ofLogWarning(LOG_NAME) << "Not yet implimented";
    return false;
}

//--------------------------------------------------------------
bool ofxOpenNI::addPlayerGenerator(){
	ofLogWarning(LOG_NAME) << "Not yet implimented";
    return false;
}

bool ofxOpenNI::removeDepthGenerator(){
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = g_Depth.StopGenerating();
    SHOW_RC(nRetVal, "Stop generating depth");
    g_bIsDepthOn = (nRetVal != XN_STATUS_OK);
    if (!g_bIsDepthOn) g_Depth.Release();
    return g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeImageGenerator(){
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = g_Image.StopGenerating();
    SHOW_RC(nRetVal, "Stop generating image");
    g_bIsImageOn = (nRetVal != XN_STATUS_OK);
    if (!g_bIsImageOn) g_Image.Release();
    return g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeInfraGenerator(){
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = g_IR.StopGenerating();
    SHOW_RC(nRetVal, "Stop generating infra");
    g_bIsIROn = (nRetVal != XN_STATUS_OK);
    if (!g_bIsIROn) g_IR.Release();
    return g_bIsIROn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeUserGenerator(){
    XnStatus nRetVal = XN_STATUS_OK;
    nRetVal = g_User.StopGenerating();
    SHOW_RC(nRetVal, "Stop generating user");
    g_bIsUserOn = (nRetVal != XN_STATUS_OK);
    if (!g_bIsUserOn) g_User.Release();
    return g_bIsUserOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::removeAudioGenerator(){
    ofLogWarning(LOG_NAME) << "Not yet implimented";
    return false;
}

//--------------------------------------------------------------
bool ofxOpenNI::removePlayerGenerator(){
    ofLogWarning(LOG_NAME) << "Not yet implimented";
    return false;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateDepthBuffers(){
    bool ok = setGeneratorResolution(g_Depth, width, height, fps);
    if (!ok) return false;
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
    bool ok = setGeneratorResolution(g_Depth, width, height, fps);
    if (!ok) return false;
    maxDepth = g_Depth.GetDeviceMaxDepth();
    depthRawPixels[0].allocate(width, height, OF_PIXELS_MONO);
    depthRawPixels[1].allocate(width, height, OF_PIXELS_MONO);
    currentDepthRawPixels = &depthRawPixels[0];
    backDepthRawPixels = &depthRawPixels[1];
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateImageBuffers(){
    bool ok = setGeneratorResolution(g_Image, width, height, fps);
    if (!ok) return false;
    imagePixels[0].allocate(width, height, OF_IMAGE_COLOR);
    imagePixels[1].allocate(width, height, OF_IMAGE_COLOR);
    currentImagePixels = &imagePixels[0];
    backImagePixels = &imagePixels[1];
    if (bUseTexture) imageTexture.allocate(width, height, GL_RGB);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateIRBuffers(){
    bool ok = setGeneratorResolution(g_IR, width, height, fps);
    if (!ok) return false;
    imagePixels[0].allocate(width, height, OF_IMAGE_GRAYSCALE);
    imagePixels[1].allocate(width, height, OF_IMAGE_GRAYSCALE);
    currentImagePixels = &imagePixels[0];
    backImagePixels = &imagePixels[1];
    if (bUseTexture) imageTexture.allocate(width, height, GL_LUMINANCE);
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::allocateUsers(){
    
    XnStatus nRetVal = XN_STATUS_OK;
    bool ok = false;
    
    // set to verbose for now
    ofSetLogLevel(CALLBACK_LOG_NAME, OF_LOG_VERBOSE);
    
    // get user generator reference
    xn::UserGenerator& userGenerator = g_User;
    
    // register user callbacks
    XnCallbackHandle User_CallbackHandler;
    XnCallbackHandle Calibration_CallbackHandler;
    
	nRetVal = userGenerator.RegisterUserCallbacks(User_NewUser, User_LostUser, this, User_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register user New/Lost callbacks");
    
	nRetVal = userGenerator.GetSkeletonCap().RegisterToCalibrationStart(UserCalibration_CalibrationStart, this, Calibration_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register user Calibration Start callback");
    
	nRetVal = userGenerator.GetSkeletonCap().RegisterToCalibrationComplete(UserCalibration_CalibrationEnd, this, Calibration_CallbackHandler);
    BOOL_ERR_RC(nRetVal, "Register user Calibration End callback");
    
    // check need for calibration
    if (userGenerator.GetSkeletonCap().NeedPoseForCalibration()){
        ofLogNotice(LOG_NAME) << "User generator DOES require pose for calibration";
        bNeedsPose = true;
        if(!userGenerator.IsCapabilitySupported(XN_CAPABILITY_POSE_DETECTION)) {
			ofLogError(LOG_NAME) << "Pose required, but not supported";
		}
        XnCallbackHandle Pose_CallbackHandler;
        nRetVal = userGenerator.GetPoseDetectionCap().RegisterToPoseDetected(UserPose_PoseDetected, this, Pose_CallbackHandler);
        BOOL_ERR_RC(nRetVal, "Register user Pose Detected callback");
        
        nRetVal = userGenerator.GetSkeletonCap().GetCalibrationPose(userCalibrationPose);
        BOOL_ERR_RC(nRetVal, "Get calibration pose");
        
        //userGenerator.GetPoseDetectionCap().StartPoseDetection("Psi", user); 
    } else {
        ofLogNotice(LOG_NAME) << "User generator DOES NOT require pose for calibration";
        bNeedsPose = false;
        //userGenerator.GetSkeletonCap().RequestCalibration(user, TRUE); 
    } 
    
	nRetVal = userGenerator.GetSkeletonCap().SetSkeletonProfile(XN_SKEL_PROFILE_ALL);
    BOOL_RC(nRetVal, "Set skeleton profile");
    
}

//--------------------------------------------------------------
void ofxOpenNI::setUseBackBuffer(bool b){
	bUseBackBuffer = b;
}

//--------------------------------------------------------------
void ofxOpenNI::setUseMaskPixels(bool b){
	bUseMaskPixels = b;
}

//--------------------------------------------------------------
void ofxOpenNI::setUsePointClouds(bool b){
	bUsePointClouds = b;
}

//--------------------------------------------------------------
void ofxOpenNI::setSmoothing(float smooth){
	if (smooth > 0.0f && smooth < 1.0f) {
		userSmoothFactor = smooth;
		if (g_bIsUserOn) {
			g_User.GetSkeletonCap().SetSmoothing(smooth);
		}
	}
}

//--------------------------------------------------------------
float ofxOpenNI::getSmoothing(){
	return userSmoothFactor;
}

//--------------------------------------------------------------
void ofxOpenNI::startPoseDetection(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
	ofLogNotice(LOG_NAME) << "Start pose detection for user" << nID;
	nRetVal = g_User.GetPoseDetectionCap().StartPoseDetection(userCalibrationPose, nID);
    SHOW_RC(nRetVal, "Get pose detection capability - start");
}


//--------------------------------------------------------------
void ofxOpenNI::stopPoseDetection(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    ofLogNotice(LOG_NAME) << "Stop pose detection for user" << nID;
	nRetVal = g_User.GetPoseDetectionCap().StopPoseDetection(nID);
    CHECK_ERR_RC(nRetVal, "Get pose detection capability - stop");
}


//--------------------------------------------------------------
void ofxOpenNI::requestCalibration(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
	ofLogNotice(LOG_NAME) << "Calibration requested for user" << nID;
	nRetVal = g_User.GetSkeletonCap().RequestCalibration(nID, TRUE);
    CHECK_ERR_RC(nRetVal, "Get skeleton capability - request calibration");
}

//--------------------------------------------------------------
void ofxOpenNI::startTracking(XnUserID nID){
    XnStatus nRetVal = XN_STATUS_OK;
    ofLogNotice(LOG_NAME) << "Start tracking user" << nID;
	nRetVal = g_User.GetSkeletonCap().StartTracking(nID);
    CHECK_ERR_RC(nRetVal, "Get skeleton capability - start tracking");
}

//--------------------------------------------------------------
bool ofxOpenNI::needsPoseForCalibration(){
	return bNeedsPose;
}

//--------------------------------------------------------------
int	ofxOpenNI::getNumUsers(){
    return currentTrackedUserIDs.size();
}

//--------------------------------------------------------------
ofxOpenNIUser&	ofxOpenNI::getUser(int nID){
    return currentTrackedUsers[currentTrackedUserIDs[nID]];
}

//--------------------------------------------------------------
void ofxOpenNI::updateUsers(){
    
    if (!g_bIsUserOn) return;
    
    // get user generator reference
    xn::UserGenerator& userGenerator = g_User;
    
	vector<XnUserID> userIDs(MAX_NUMBER_USERS);
	XnUInt16 maxNumUsers = MAX_NUMBER_USERS;
	userGenerator.GetUsers(&userIDs[0], maxNumUsers);
    
	set<XnUserID> trackedUserIDs;
    
	for (int i = 0; i < MAX_NUMBER_USERS; ++i) {
		if (userGenerator.GetSkeletonCap().IsTracking(userIDs[i])) {
			ofxOpenNIUser & user = currentTrackedUsers[userIDs[i]];
			user.id = userIDs[i];
			XnPoint3D center;
			userGenerator.GetCoM(userIDs[i], center);
			user.center = toOf(center);
            
			for (int j=0; j<ofxOpenNIUser::NumLimbs; j++){
				XnSkeletonJointPosition a,b;
				userGenerator.GetSkeletonCap().GetSkeletonJointPosition(user.id, user.limbs[j].start_joint, a);
				userGenerator.GetSkeletonCap().GetSkeletonJointPosition(user.id, user.limbs[j].end_joint, b);
				userGenerator.GetSkeletonCap().GetSkeletonJointOrientation(user.id,user.limbs[j].start_joint, user.limbs[j].orientation);
				if (a.fConfidence < 0.3f || b.fConfidence < 0.3f){
					user.limbs[j].found = false;
					continue;
				}
                
				user.limbs[j].found = true;
				user.limbs[j].begin = worldToProjective(a.position);
				user.limbs[j].end = worldToProjective(b.position);
				user.limbs[j].worldBegin = toOf(a.position);
				user.limbs[j].worldEnd = toOf(b.position);
			}
            
			if (bUsePointClouds) updatePointClouds(user);
			if (bUseMaskPixels) updateUserPixels(user);
            
			trackedUserIDs.insert(user.id);
		}
	}
    
	set<XnUserID>::iterator it;
	for (it = previousTrackedUserIDs.begin(); it != previousTrackedUserIDs.end(); it++){
		if (trackedUserIDs.find(*it) == trackedUserIDs.end()){
			currentTrackedUsers.erase(*it);
		}
	}

	previousTrackedUserIDs = trackedUserIDs;
	currentTrackedUserIDs.assign(previousTrackedUserIDs.begin(), previousTrackedUserIDs.end());
    
	//if (useMaskPixels) updateUserPixels();
}

//--------------------------------------------------------------
void ofxOpenNI::updatePointClouds(ofxOpenNIUser & user){
    
	const XnRGB24Pixel*	pColor;
	const XnDepthPixel*	pDepth = g_DepthMD.Data();
    
	if (g_bIsImageOn) {
		pColor = g_ImageMD.RGB24Data();
	}
    
	xn::SceneMetaData smd;
    //if (!smd.IsDataNew()) return;
    
	unsigned short *userPix;
    
	if (g_User.GetUserPixels(user.id, smd) == XN_STATUS_OK) {
		userPix = (unsigned short*)smd.Data();
	}
    
	int step = 1;
	int nIndex = 0;
    
	user.pointCloud.getVertices().clear();
	user.pointCloud.getColors().clear();
	user.pointCloud.setMode(OF_PRIMITIVE_POINTS);
    
	for (int nY = 0; nY < getHeight(); nY += step) {
		for (int nX = 0; nX < getWidth(); nX += step, nIndex += step) {
			if (userPix[nIndex] == user.id) {
				user.pointCloud.addVertex(ofPoint( nX,nY,pDepth[nIndex] ));
				if(g_bIsImageOn){
					user.pointCloud.addColor(ofColor(pColor[nIndex].nRed,pColor[nIndex].nGreen,pColor[nIndex].nBlue));
				}else{
					user.pointCloud.addColor(ofFloatColor(1,1,1));
				}
			}
		}
	}
}

//--------------------------------------------------------------
void ofxOpenNI::updateUserPixels(ofxOpenNIUser & user){
    
	xn::SceneMetaData smd;
	unsigned short *userPix;
    
	if (g_User.GetUserPixels(user.id, smd) == XN_STATUS_OK) { //	GetUserPixels is supposed to take a user ID number,
		userPix = (unsigned short*)smd.Data();					//  but you get the same data no matter what you pass.
	}															//	userPix actually contains an array where each value
    //  corresponds to the user being tracked.
    //  Ie.,	if userPix[i] == 0 then it's not being tracked -> it's the background!
    //			if userPix[i] > 0 then the pixel belongs to the user who's value IS userPix[i]
    //  // (many thanks to ascorbin who's code made this apparent to me)
    
	user.maskPixels.allocate(getWidth(), getHeight(), OF_IMAGE_GRAYSCALE);
    
	for (int i =0 ; i < getWidth() * getHeight(); i++) {
		if (userPix[i] == user.id) {
			user.maskPixels[i] = 255;
		} else {
			user.maskPixels[i] = 0;
		}
        
	}
}

//--------------------------------------------------------------
void ofxOpenNI::updateFrame(){
    
	//if (bIsThreaded) lock(); // with this here I get ~30 fps with 2 Kinects
	
    if (!bIsContextReady) return;
    
    g_Context.WaitAnyUpdateAll();
    
//    this doesn't seem as fast/smooth, but is more 'correct':
//    if (g_bIsUserOn) {
//        g_Context.WaitOneUpdateAll(g_User);
//    } else {
//        g_Context.WaitAnyUpdateAll();
//    }
    //if (bIsThreaded) lock(); // with this her I get ~300-400+ fps with 2 Kinects!
    
	if (g_bIsDepthOn && g_Depth.IsDataNew()) g_Depth.GetMetaData(g_DepthMD);
	if (g_bIsImageOn && g_Image.IsDataNew()) g_Image.GetMetaData(g_ImageMD);
	if (g_bIsIROn && g_IR.IsDataNew()) g_IR.GetMetaData(g_IrMD);
	
    if (bIsThreaded) lock(); // with this her I get ~400-500+ fps with 2 Kinects!
    
    if (g_bIsDepthOn) generateDepthPixels();
	if (g_bIsImageOn) generateImagePixels();
	if (g_bIsIROn) generateIRPixels();

// NB: Below info is from my old single context setup - need to retest with this new multicontext setup!    

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
        if (g_bIsDepthOn){
            swap(backDepthPixels, currentDepthPixels);
            if (g_bIsDepthRawOnOption){
                swap(backDepthRawPixels, currentDepthRawPixels);
            }
        }
        if (g_bIsImageOn || g_bIsIROn){
            swap(backImagePixels, currentImagePixels);
        }
    }
	
	
	bNewPixels = true;
	
	if (bIsThreaded) unlock();
}

//--------------------------------------------------------------
void ofxOpenNI::update(){

    if (!bIsContextReady) return;
    
	if (!bIsThreaded){
        if (shutdown) {
            stop();
            return;
        }
		updateFrame();
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
		if (bUseTexture && (g_bIsImageOn || g_bIsIROn)){
            if (bUseBackBuffer) {
                imageTexture.loadData(*currentImagePixels);
            } else {
                imageTexture.loadData(*backImagePixels);
            }
		}
        
        updateUsers();
        
		bNewPixels = false;
		bNewFrame = true;

	}

	if (bIsThreaded) unlock();
}

//--------------------------------------------------------------
void ofxOpenNI::threadedFunction(){
	while(isThreadRunning()){
        lock();
        if (shutdown) {
            stop();
            return;
        }
        unlock();
		updateFrame();
	}
}

//--------------------------------------------------------------
bool ofxOpenNI::isNewFrame(){
	return bNewFrame;
}

//--------------------------------------------------------------
bool ofxOpenNI::toggleCalibratedRGBDepth(){
	
	if (!g_Image.IsValid()){
		ofLogError(LOG_NAME) << "No Image generator found: cannot register viewport";
		return false;
	}
	
	// Toggle registering view point to image map
	if (g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)){
		
		if (g_Depth.GetAlternativeViewPointCap().IsViewPointAs(g_Image)){
			disableCalibratedRGBDepth();
		} else {
			enableCalibratedRGBDepth();
		}
		
	} else return false;
	
	return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::enableCalibratedRGBDepth(){
	if (!g_Image.IsValid()){
		ofLogError(LOG_NAME) << "No Image generator found: cannot register viewport";
		return false;
	}
	
	// Register view point to image map
	if (g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)){
		
		XnStatus nRetVal = g_Depth.GetAlternativeViewPointCap().SetViewPoint(g_Image);
		SHOW_RC(nRetVal, "Register viewpoint depth to RGB")
		if (nRetVal!=XN_STATUS_OK) return false;
	} else {
		ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
		return false;
	}
	
	return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::disableCalibratedRGBDepth(){
	// Unregister view point from (image) any map
	if (g_Depth.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)){
		XnStatus nRetVal = g_Depth.GetAlternativeViewPointCap().ResetViewPoint();
		SHOW_RC(nRetVal, "Unregister viewpoint depth to RGB");
		if (nRetVal!=XN_STATUS_OK) return false;
	} else {
		ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
		return false;
	}
	
	return true;
}

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
    
    int generatorCount = 0;
    if (g_bIsDepthOn) generatorCount++;
    if (g_bIsImageOn) generatorCount++;
    if (g_bIsIROn) generatorCount++;
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
    
    ofPushMatrix();
    if (g_bIsDepthOn) drawDepth();
    if (g_bIsUserOn) drawUsers();
    ofTranslate(getWidth(), 0.0f);
    if (g_bIsImageOn) drawImage();
    if (g_bIsIROn) drawImage();
    ofPopMatrix();
    
    ofPopMatrix();
    ofPopStyle();
}

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

//--------------------------------------------------------------
void ofxOpenNI::drawUsers(){
    if (bIsContextReady) drawUsers(0.0f, 0.0f, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawUsers(float x, float y){
	if (bIsContextReady) drawUsers(x, y, getWidth(), getHeight());
}

//--------------------------------------------------------------
void ofxOpenNI::drawUsers(float x, float y, float w, float h){
	if (!bIsContextReady) return;
    ofPushStyle();
    for(int i = 0;  i < (int)currentTrackedUserIDs.size(); ++i){
        drawUser(x, y, w, h, i);
    }
	ofPopStyle();
}

//--------------------------------------------------------------
void ofxOpenNI::drawUser(int nID){
	drawUser(0.0f, 0.0f, getWidth(), getHeight(), nID);
}

//--------------------------------------------------------------
void ofxOpenNI::drawUser(float x, float y, int nID){
	drawUser(x, y, getWidth(), getHeight(), nID);
}

//--------------------------------------------------------------
void ofxOpenNI::drawUser(float x, float y, float w, float h, int nID){
	if(nID - 1 > (int)currentTrackedUserIDs.size()) return;
    ofPushMatrix();
    ofTranslate(x, y);
    ofScale(w/getWidth(), h/getHeight(), 1.0f);
	currentTrackedUsers[currentTrackedUserIDs[nID]].draw();
    ofPopMatrix();
}

//--------------------------------------------------------------
void ofxOpenNI::setDepthColoring(DepthColoring coloring){
	depthColoring = coloring;
}

//--------------------------------------------------------------
void ofxOpenNI::generateIRPixels(){
	const XnIRPixel* pImage = g_IrMD.Data();
    unsigned char * ir_pixels = new unsigned char[g_IrMD.XRes() * g_IrMD.YRes()];
	for (int i = 0; i < g_IrMD.XRes() * g_IrMD.YRes(); i++){
		ir_pixels[i] = pImage[i]/4;
	}
	backImagePixels->setFromPixels(ir_pixels, g_IrMD.XRes(), g_IrMD.YRes(), OF_IMAGE_GRAYSCALE);
    delete ir_pixels;
}

//--------------------------------------------------------------
void ofxOpenNI::generateImagePixels(){
	const XnUInt8* pImage = g_ImageMD.Data();
	backImagePixels->setFromPixels(pImage, g_ImageMD.XRes(), g_ImageMD.YRes(), OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofxOpenNI::generateDepthPixels(){
	// get the pixels
	const XnDepthPixel* depth = g_DepthMD.Data();
	
	if (g_DepthMD.FrameID() == 0) return;
	
	// copy raw values
	if (g_bIsDepthRawOnOption){
		backDepthRawPixels->setFromPixels(depth, g_DepthMD.XRes(), g_DepthMD.YRes(), OF_IMAGE_COLOR);
	}
	
	// copy depth into texture-map
	float max;
	for (XnUInt16 y = g_DepthMD.YOffset(); y < g_DepthMD.YRes() + g_DepthMD.YOffset(); y++){
		unsigned char * texture = backDepthPixels->getPixels() + y * g_DepthMD.XRes() * 4 + g_DepthMD.XOffset() * 4;
		for (XnUInt16 x = 0; x < g_DepthMD.XRes(); x++, depth++, texture += 4){
			XnUInt8 red = 0;
			XnUInt8 green = 0;
			XnUInt8 blue = 0;
			XnUInt8 alpha = 255;
			
			XnUInt16 col_index;
			
			switch (depthColoring){
				case COLORING_PSYCHEDELIC_SHADES:
					alpha *= (((XnFloat)(*depth % 10) / 20) + 0.5);
				case COLORING_PSYCHEDELIC:
					switch ((*depth/10) % 10){
						case 0:
							red = 255;
							break;
						case 1:
							green = 255;
							break;
						case 2:
							blue = 255;
							break;
						case 3:
							red = 255;
							green = 255;
							break;
						case 4:
							green = 255;
							blue = 255;
							break;
						case 5:
							red = 255;
							blue = 255;
							break;
						case 6:
							red = 255;
							green = 255;
							blue = 255;
							break;
						case 7:
							red = 127;
							blue = 255;
							break;
						case 8:
							red = 255;
							blue = 127;
							break;
						case 9:
							red = 127;
							green = 255;
							break;
					}
					break;
				case COLORING_RAINBOW:
					col_index = (XnUInt16)(((*depth) / (maxDepth / 256)));
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case COLORING_CYCLIC_RAINBOW:
					col_index = (*depth % 256);
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case COLORING_BLUES:
					// 3 bytes of depth: black (R0G0B0) >> blue (001) >> cyan (011) >> white (111)
					max = 256+255+255;
					col_index = (XnUInt16)(((*depth) / (maxDepth / max)));
					if ( col_index < 256 )
					{
						blue	= col_index;
						green	= 0;
						red		= 0;
					}
					else if ( col_index < (256+255) )
					{
						blue	= 255;
						green	= (col_index % 256) + 1;
						red		= 0;
					}
					else if ( col_index < (256+255+255) )
					{
						blue	= 255;
						green	= 255;
						red		= (col_index % 256) + 1;
					}
					else
					{
						blue	= 255;
						green	= 255;
						red		= 255;
					}
					break;
				case COLORING_GREY:
					max = 255;	// half depth
				{
					XnUInt8 a = (XnUInt8)(((*depth) / (maxDepth / max)));
					red		= a;
					green	= a;
					blue	= a;
				}
					break;
				case COLORING_STATUS:
					// This is something to use on installations
					// when the end user needs to know if the camera is tracking or not
					// The scene will be painted GREEN if status == true
					// The scene will be painted RED if status == false
					// Usage: declare a global bool status and that's it!
					// I'll keep it commented so you dont have to have a status on every project
#if 0
				{
					extern bool status;
					max = 255;	// half depth
					XnUInt8 a = 255 - (XnUInt8)(((*depth) / (maxDepth / max)));
					red		= ( status ? 0 : a);
					green	= ( status ? a : 0);
					blue	= 0;
				}
#endif
					break;
			}
			
			texture[0] = red;
			texture[1] = green;
			texture[2] = blue;
			
			if (*depth == 0)
				texture[3] = 0;
			else
				texture[3] = alpha;
		}
	}
	
	
}

//--------------------------------------------------------------
bool ofxOpenNI::isContextReady(){
	return bIsContextReady;
}

//--------------------------------------------------------------
int ofxOpenNI::getNumDevices(){
    if (numDevices == 0) initDevice();
	return numDevices;
}

//--------------------------------------------------------------
int ofxOpenNI::getDeviceID(){
	return instanceID;
}

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
	return g_Image;;
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNI::getIRGenerator(){
	return g_IR;;
}

//--------------------------------------------------------------
xn::UserGenerator& ofxOpenNI::getUserGenerator(){
	return g_User;;
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNI::getAudioGenerator(){
	return g_Audio;
}

//--------------------------------------------------------------
xn::Player& ofxOpenNI::getPlayer(){
	return g_Player;
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
	return g_IrMD;
}

//--------------------------------------------------------------
xn::AudioMetaData& ofxOpenNI::getAudioMetaData(){
	return g_AudioMD;
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getDepthPixels(){
	if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	return *currentDepthPixels;
}

//--------------------------------------------------------------
ofShortPixels& ofxOpenNI::getDepthRawPixels(){
	if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	if (!g_bIsDepthRawOnOption){
		ofLogWarning(LOG_NAME) << "g_bIsDepthRawOnOption was disabled, enabling raw pixels";
		g_bIsDepthRawOnOption = true;
	}
	return *currentDepthRawPixels;
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getImagePixels(){
	if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
	return *currentImagePixels;
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
    
    if (g_bIsIROn) {
        ok = allocateIRBuffers();
        if(!ok) {
            width = oW; height = oH; fps = oF;
            allocateIRBuffers();
        }
    }
    
    return ok;
}

//--------------------------------------------------------------
bool ofxOpenNI::setGeneratorResolution(MapGenerator & generator, int w, int h, int f){
    //if (bIsThreaded) Poco::ScopedLock<ofMutex> lock(mutex);
    
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
	}else if (g_bIsIROn){
		return g_IrMD.XRes();
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
	}else if (g_bIsIROn){
		return g_IrMD.YRes();
	} else {
		ofLogWarning(LOG_NAME) << "getHeight() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

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
	XnPoint3D *out =&projective[0];
	
	//lock();
	const XnDepthPixel* d = currentDepthRawPixels->getPixels();
	unsigned int pixel;
	for (int i=0; i<nPoints; ++i){
		pixel  = (int)c[i].x + (int)c[i].y * g_DepthMD.XRes();
		if (pixel >= g_DepthMD.XRes() * g_DepthMD.YRes())
			continue;
		
		projective[i].X = c[i].x;
		projective[i].Y = c[i].y;
		projective[i].Z = float(d[pixel]) / 1000.0f;
	}
	//unlock();
	
	g_Depth.ConvertProjectiveToRealWorld(nPoints,&projective[0], (XnPoint3D*)&w[0]);
	
}

// USER GENERATOR CALLBACKS
// =============================================================

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::User_NewUser(xn::UserGenerator& rGenerator, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "New User" << nID;
	if(openNI->needsPoseForCalibration()) {
		openNI->startPoseDetection(nID);
	} else {
		openNI->requestCalibration(nID);
	}
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::User_LostUser(xn::UserGenerator& rGenerator, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "Lost user" << nID;
	rGenerator.GetSkeletonCap().Reset(nID);
    
}

//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserPose_PoseDetected(xn::PoseDetectionCapability& rCapability, const XnChar* strPose, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "Pose" << strPose << "detected for user" << nID;
	openNI->requestCalibration(nID);
	openNI->stopPoseDetection(nID);
}


//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nID, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "Calibration start for user" << nID;
}


//--------------------------------------------------------------
void XN_CALLBACK_TYPE ofxOpenNI::UserCalibration_CalibrationEnd(xn::SkeletonCapability& rCapability, XnUserID nID, XnCalibrationStatus bSuccess, void* pCookie){
    ofxOpenNI* openNI = static_cast<ofxOpenNI*>(pCookie);
	ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "Calibration end for user..." << nID;
	if(bSuccess == XN_CALIBRATION_STATUS_OK) {
		ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "...success" << nID;
		openNI->startTracking(nID);
	} else {
        ofLogVerbose(CALLBACK_LOG_NAME) << "Device[" << openNI->getDeviceID() << "]" << "...fail" << nID;
		if(openNI->needsPoseForCalibration()) {
			openNI->startPoseDetection(nID);
		} else {
			openNI->requestCalibration(nID);
		}
	}
}