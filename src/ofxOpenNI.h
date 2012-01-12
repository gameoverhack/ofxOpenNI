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

#include "ofxOpenNIContext.h"
#include "ofxOpenNIUtils.h"
#include "ofxOpenNIUser.h"

#include "ofThread.h"

using namespace xn;

class ofxOpenNI : public ofThread {
	
public:
	
	enum DepthColoring {
		COLORING_PSYCHEDELIC_SHADES = 0,
		COLORING_PSYCHEDELIC,
		COLORING_RAINBOW,
		COLORING_CYCLIC_RAINBOW,
		COLORING_BLUES,
		COLORING_GREY,
		COLORING_STATUS,
		COLORING_COUNT
	};
	
	ofxOpenNI();
	~ofxOpenNI();
	
	bool setup(bool threaded = true);
	bool setup(string xmlFilePath, bool threaded = true);
	
	bool addDepthGenerator();
	bool addImageGenerator();
	bool addInfraGenerator();
	bool addAudioGenerator();
	bool addUserGenerator();
	bool addPlayerGenerator();

	void update();
	
	void drawDepth();
	void drawDepth(float x, float y);
	void drawDepth(float x, float y, float w, float h);
	
	void drawImage();
	void drawImage(float x, float y);
	void drawImage(float x, float y, float w, float h);
	
	float getWidth();
	float getHeight();
	
	int getNumDevices();
	
	bool isNewFrame();
	
	void setUseTexture(bool useTexture);
	void setDepthColoring(DepthColoring coloring);
	
	bool toggleCalibratedRGBDepth();
	bool enableCalibratedRGBDepth();
	bool disableCalibratedRGBDepth();
	
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
	
	xn::Device& getDevice();
	xn::DepthGenerator& getDepthGenerator();
	xn::ImageGenerator& getImageGenerator();
	xn::IRGenerator& getIRGenerator();
	xn::AudioGenerator& getAudioGenerator();
	xn::Player& getPlayer();
	
	xn::DepthMetaData& getDepthMetaData();
	xn::ImageMetaData& getImageMetaData();
	xn::IRMetaData& getIRMetaData();
	xn::AudioMetaData& getAudioMetaData();
	
    string LOG_NAME;

protected:
	
	void threadedFunction();
    
private:
	
	//void openCommon();
	//void initConstants();
	void readFrame();
	void generateDepthPixels();
	void generateImagePixels();
	void generateIRPixels();
	
	void allocateDepthBuffers();
	void allocateDepthRawBuffers();
	void allocateImageBuffers();
	void allocateIRBuffers();
    void allocateUsers();
	
	bool bIsThreaded;
	
	bool g_bIsDepthOn;
	bool g_bIsImageOn;
	bool g_bIsIROn;
    bool g_bIsUserOn;
	bool g_bIsAudioOn;
	bool g_bIsPlayerOn;
	bool g_bIsDepthRawOnOption;
	
	bool bUseTexture;
	bool bNewPixels;
	bool bNewFrame;
    
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
	
	// meta data
	xn::DepthMetaData g_DepthMD;
	xn::ImageMetaData g_ImageMD;
	xn::IRMetaData	g_IrMD;
	xn::AudioMetaData g_AudioMD;
	
	// generators/nodes
	xn::MockDepthGenerator mockDepth;
	
    // user callback handlers
    static void XN_CALLBACK_TYPE User_NewUser(xn::UserGenerator& rGenerator, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE User_LostUser(xn::UserGenerator& rGenerator, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE UserPose_PoseDetected(xn::PoseDetectionCapability& rCapability, const XnChar* strPose, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE UserCalibration_CalibrationStart(xn::SkeletonCapability& capability, XnUserID nID, void* pCookie);
    static void XN_CALLBACK_TYPE UserCalibration_CalibrationEnd(xn::SkeletonCapability& rCapability, XnUserID nID, XnCalibrationStatus bSuccess, void* pCookie);
    
	int instanceID;
    
    // block copy ctor and assignment operator
    ofxOpenNI(const ofxOpenNI& other);
    ofxOpenNI& operator=(const ofxOpenNI&);
	
};

#endif
