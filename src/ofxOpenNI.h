/*
 * ofxOpenNI.h
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

#ifndef	_H_OFXOPENNI
#define _H_OFXOPENNI

#include <set.h>
#include <map.h>

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <XnTypes.h>

#include "ofLog.h"
#include "ofConstants.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofThread.h"
#include "ofEvents.h"

#include "ofxOpenNIUser.h"
#include "ofxOpenNIUtils.h"

using namespace xn;

class ofxOpenNI : public ofThread {
	
public:
	
	ofxOpenNI();
	~ofxOpenNI();
	
	bool setup(bool threaded = true);
	bool setup(string xmlFilePath, bool threaded = true);
	
    void stop();
    
	bool addDepthGenerator();
	bool addImageGenerator();
	bool addInfraGenerator();
	bool addUserGenerator();
    bool addGestureGenerator();
    bool addAudioGenerator();
	bool addPlayerGenerator();

    bool removeDepthGenerator();
    bool removeImageGenerator();
    bool removeInfraGenerator();
    bool removeUserGenerator();
    bool removeGestureGenerator();
    bool removeAudioGenerator();
    bool removePlayerGenerator();
    
	void update();
	
    void drawDebug();
	void drawDebug(float x, float y);
	void drawDebug(float x, float y, float w, float h);
    
	void drawDepth();
	void drawDepth(float x, float y);
	void drawDepth(float x, float y, float w, float h);
	
	void drawImage();
	void drawImage(float x, float y);
	void drawImage(float x, float y, float w, float h);
	
    void drawSkeletons();
    void drawSkeletons(float x, float y);
    void drawSkeletons(float x, float y, float w, float h);
    
    void drawSkeleton(int nID);
    void drawSkeleton(float x, float y, int nID);
    void drawSkeleton(float x, float y, float w, float, int nID);
    
    bool setResolution(int w, int h, int fps);

	float getWidth();
	float getHeight();
	
    void setLogLevel(XnLogSeverity logLevel);
    void setLogLevel(ofLogLevel logLevel);
    
	int getNumDevices();
	
    bool isContextReady();
	bool isNewFrame();
    
    bool isDepthOn();
	bool isImageOn();
    bool isInfraOn();
    bool isUserOn();
    bool isGestureOn();
    bool isAudioOn();
    
	void setUseTexture(bool useTexture);
	void setDepthColoring(DepthColoring coloring);
	
    void setUseBackBuffer(bool b);
    bool getUseBackBuffer();
    
    ofxOpenNIUser&	getTrackedUser(int index); // only returns tracked users upto getNumTrackedUsers()
    int	getNumTrackedUsers();
    
    ofxOpenNIUser& getUser(XnUserID nID); // finds a user if it exists (whether tracked or not)
    void setMaxNumUsers(int numUsers);
    int	getMaxNumUsers();
    
    void setUseMaskTextureAllUsers(bool b);
    void setUseMaskPixelsAllUsers(bool b);
    void setUsePointCloudsAllUsers(bool b);
    void setPointCloudDrawSizeAllUsers(int size);
    void setPointCloudResolutionAllUsers(int resolution);
    
    void setBaseUserClass(ofxOpenNIUser & user);
    
    void resetUserTracking(XnUserID nID, bool forceImmediateRestart = false);
    
	void setUserSmoothing(float smooth);
	float getUserSmoothing();
    
    bool getAutoUserCalibrationPossible();
    
    // gesture methods
    bool addGesture(string niteGestureName, ofPoint LeftBottomNear = NULL, ofPoint RightTopFar = NULL);
    bool removeGesture(string niteGestureName);
    
    vector<string> & getAvailableGestures();
    bool isGestureAvailable(string niteGestureName);
    
    void setMinTimeBetweenGestures(int millis);
    int getMinTimeBetweenGestures();
    
    // generator 'capabilities'
    void toggleRegister();
	void setRegister(bool b);
    bool getRegister();
    
    void toggleMirror();
	void setMirror(bool b);
    bool getMirror();
    
    void toggleSync();
	void setSync(bool b);
    bool getSync();
    
	ofPixels& getDepthPixels();
	ofShortPixels& getDepthRawPixels();
	ofPixels& getImagePixels();
	
	ofTexture& getDepthTextureReference();
	ofTexture& getimageTextureReference();
	
	ofPoint worldToProjective(const ofPoint & p);
	ofPoint worldToProjective(const XnVector3D & p);
	
	ofPoint projectiveToWorld(const ofPoint & p);
	ofPoint projectiveToWorld(const XnVector3D & p);
	
	ofPoint cameraToWorld(const ofVec2f& c);
	void cameraToWorld(const vector<ofVec2f>& c, vector<ofVec3f>& w);
	
    int getDeviceID();
    
    xn::Context& getContext();
	xn::Device& getDevice();
   
	xn::DepthGenerator& getDepthGenerator();
	xn::ImageGenerator& getImageGenerator();
	xn::IRGenerator& getIRGenerator();
    xn::UserGenerator& getUserGenerator();
	xn::AudioGenerator& getAudioGenerator();
	xn::Player& getPlayer();
	
	xn::DepthMetaData& getDepthMetaData();
	xn::ImageMetaData& getImageMetaData();
	xn::IRMetaData& getIRMetaData();
	xn::AudioMetaData& getAudioMetaData();
	
    string LOG_NAME;
    
    ofEvent<ofxOpenNIUserEvent> userEvent;
    ofEvent<ofxOpenNIGestureEvent> gestureEvent;

protected:
	
	void threadedFunction();
    
private:
    
	//void openCommon();
	//void initConstants();
    bool initContext();
    bool initDevice();
    
    bool setGeneratorResolution(MapGenerator & generator, int w, int h, int f);
    
    bool addLicence(string sVendor, string sKey);
    void logErrors(xn::EnumerationErrors & errors);
    
	void updateGenerators();
    
	bool allocateDepthBuffers();
	bool allocateDepthRawBuffers();
	bool allocateImageBuffers();
	bool allocateIRBuffers();
    bool allocateUsers();
    bool allocateGestures();
    
    void generateDepthPixels();
	void generateImagePixels();
	void generateIRPixels();
    void generateUserTracking();
    void generateUserPixels(ofxOpenNIUser & user);
	void generatePointClouds(ofxOpenNIUser & user);
	
	bool g_bIsDepthOn;
	bool g_bIsImageOn;
	bool g_bIsInfraOn;
    bool g_bIsUserOn;
    bool g_bIsGestureOn;
	bool g_bIsAudioOn;
	bool g_bIsPlayerOn;
	bool g_bIsDepthRawOnOption;
	
    bool bIsThreaded;
    bool bIsContextReady;
    bool bIsDeviceReady;
    bool bIsShuttingDown;
    
    bool bUseBackBuffer;
	bool bUseTexture;
    
    bool bUseRegistration;
    bool bUseMirror;
    bool bUseSync;
    
    bool bAutoCalibrationPossible;
    
	bool bNewPixels;
	bool bNewFrame;
    bool bUsePointClouds;
    bool bUseMaskPixels;
    
    int numDevices;
    
    int width;
    int height;
    int fps;
    
	// depth
	ofTexture depthTexture;
	ofPixels depthPixels[2];
	ofPixels* backDepthPixels;
	ofPixels* currentDepthPixels;
	DepthColoring depthColoring;
	float maxDepth;
	
	// depth raw
	ofShortPixels depthRawPixels[2];
	ofShortPixels* backDepthRawPixels;
	ofShortPixels* currentDepthRawPixels;
	
	// image
	ofTexture imageTexture;
	ofPixels imagePixels[2];
	ofPixels* backImagePixels;
	ofPixels* currentImagePixels;
	
    // generators
    xn::Context g_Context;
	xn::Device g_Device;
	xn::DepthGenerator g_Depth;
	xn::ImageGenerator g_Image;
	xn::IRGenerator g_Infra;
	xn::UserGenerator g_User;
    xn::GestureGenerator g_Gesture;
	xn::AudioGenerator g_Audio;
	xn::Player g_Player;
    
	// meta data
	xn::DepthMetaData g_DepthMD;
	xn::ImageMetaData g_ImageMD;
	xn::IRMetaData	g_InfraMD;
    xn::SceneMetaData g_SceneMD;
	xn::AudioMetaData g_AudioMD;
	
	// generators/nodes
	xn::MockDepthGenerator mockDepth;
	
    // user callback handlers
    static void XN_CALLBACK_TYPE UserCB_handleNewUser(xn::UserGenerator& userGenerator, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE UserCB_handleLostUser(xn::UserGenerator& userGenerator, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE UserCB_handlePoseDetected(xn::PoseDetectionCapability& poseDetectionCapability, const XnChar* strPose, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE UserCB_handleCalibrationStart(xn::SkeletonCapability& skeletonCapability, XnUserID nID, void* pCookie);
//    static void XN_CALLBACK_TYPE UserCB_handleCalibrationProgress(xn::SkeletonCapability& skeletonCapability, XnUserID nID, XnCalibrationStatus calibrationStatus, void* pCookie);
    static void XN_CALLBACK_TYPE UserCB_handleCalibrationEnd(xn::SkeletonCapability& skeletonCapability, XnUserID nID, XnCalibrationStatus calibrationStatus, void* pCookie);
    
    // user tracking functions
    XnChar	userCalibrationPose[20];
    void stopTrackingUser(XnUserID nID);
    void startTrackingUser(XnUserID nID);
    void requestCalibration(XnUserID nID);
    void startPoseDetection(XnUserID nID);
    void stopPoseDetection(XnUserID nID);
    
    // user storage
	map<XnUserID, ofxOpenNIUser> currentTrackedUsers;
	vector<XnUserID> currentTrackedUserIDs;
    
    ofxOpenNIUser baseUser;
    
    int maxNumUsers;
    float userSmoothFactor;

    // gesture storage
    vector<string> availableGestures;
    //multimap<string, XnBoundingBox3D> currentGestureAreas;
    ofxOpenNIGestureEvent lastGestureEvent;
    int minTimeBetweenGestures;
    
    // gesture callbacks
    static void XN_CALLBACK_TYPE GestureCB_handleGestureRecognized(xn::GestureGenerator& gestureGenerator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie);
    static void XN_CALLBACK_TYPE GestureCB_handleGestureProgress(xn::GestureGenerator& gestureGenerator, const XnChar* strGesture, const XnPoint3D* pIDPosition, XnFloat fProgress, void* pCookie);
    
	int instanceID;
    ofLogLevel logLevel;
    
    // block copy ctor and assignment operator
    ofxOpenNI(const ofxOpenNI& other);
    ofxOpenNI& operator=(const ofxOpenNI&);
	
};

#endif
