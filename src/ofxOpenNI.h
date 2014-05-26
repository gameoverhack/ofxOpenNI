/*
 * ofxOpenNI.h
 *
 * Copyright 2011-2013 (c) Matthew Gingold [gameover] http://gingold.com.au
 *
 * PermUsesion Use hereby granted, free of charge, to any person
 * obtaining a copy of thUse software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publUseh, dUsetribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software Use furnUsehed to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and thUse permUsesion notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE Use PROVIDED "AS Use", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWUseE, ARUseING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef	_H_OFXOPENNI
#define _H_OFXOPENNI

#include "ofMain.h"
#include "NiTE.h"

#include "ofxOpenNIUtils.h"
#include "ofxOpenNITypes.h"

#define MOTION_DETECTION_FPS                10      // FPS of motion detecion capturing
#define MOTION_DETECTION_CACHE_LENGTH_MS    500     // Cache length in ms to calculate average speed

using namespace openni;
using namespace nite;

class ofxOpenNI : public ofThread {
    
public:
    
    ofxOpenNI();
    ~ofxOpenNI();
    
    
    bool setup(int _deviceid = -1);
    
    void start();
    void stop();
    
    bool addDepthStream();
	bool addImageStream();
//	bool addInfraGenerator();
	bool addUserTracker(float _smoothing = 0.6);
//    bool addGestureGenerator();
    bool addHandsTracker();
//    bool addAudioGenerator();
    
//    bool removeDepthGenerator();
//    bool removeImageGenerator();
//    bool removeInfraGenerator();
//    bool removeUserGenerator();
//    bool removeGestureGenerator();
//    bool removeHandsGenerator();
//    bool removeAudioGenerator();
    
    void update();
    void draw();
    
    bool isDepthFrameNew();
    bool isImageFrameNew();
    
    map<int, ofxOpenNIUser> skeletonData();
    ofTexture& depthData();
    ofTexture& imageData();
    
    bool bUseDepth;
	bool bUseImage;
    
    void setMotionThresholds(float _threshold_motion, float _threshold_nomotion, float _cache_length);

    
protected:
    
    void updateGenerators();
    
    void threadedFunction();
    
    Device device;
    
    void allocateDepthBuffers();
	void allocateImageBuffers();
    
    void updateDepthFrame();
	void updateImageFrame();
    void updateUserFrame();
    void updateHandFrame();
    
    VideoStream depthStream;
    VideoStream imageStream;
    
    VideoFrameRef depthFrame;
    VideoFrameRef imageFrame;
    
    HandTracker handTracker;
    HandTrackerFrameRef handFrame;
    map<int, ofxOpenNIHand> trackedHands;
    
    UserTracker userTracker;
    UserTrackerFrameRef userFrame;
    map<int, ofxOpenNIUser> trackedUsers;
    
    ofTexture depthTexture;
    ofTexture imageTexture;
    
    ofPixels depthPixels;
    ofPixels imagePixels;
    
    float depthWidth;
    float depthHeight;
    
    float imageWidth;
    float imageHeight;
    
    bool bIsDepthFrameNew;
    bool bIsImageFrameNew;
    
    bool bUseNite;
    bool bUseDevice;
   

	bool bUseInfra;
    bool bUseUsers;
    bool bUseGesture;
    bool bUseHands;
	bool bUseAudio;
	bool bUseDepthRaw;
    bool bUseRecord;
    bool bUsePlayer;
    
    
    /* Motion Detection */
    float _motion_detection_rate;
    int _motion_detection_count;
    float _md_threshold_motion;
    float _md_threshold_nomotion;
//    float _md_threshold_nomotion_current;
    
};

#endif
