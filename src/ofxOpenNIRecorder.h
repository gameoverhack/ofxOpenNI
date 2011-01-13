#pragma once

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"
#include "ofxOpenNIMacros.h"
#include "ofxUserGenerator.h"

class ofxOpenNIRecorder {
public:
	ofxOpenNIRecorder();
	~ofxOpenNIRecorder();
	
	void setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepth, ofxImageGenerator* pImage);
	
	bool startRecord(string fname);
	
	bool stopRecord();


private:
	ofxOpenNIContext* context;	
	ofxDepthGenerator* depth;
	ofxImageGenerator* image;
	ofxUserGenerator* user;
	bool is_recording;	
	xn::Recorder recorder;
};
