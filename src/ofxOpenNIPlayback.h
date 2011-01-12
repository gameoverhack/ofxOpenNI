#pragma once

#include "ofxOpenNI.h"
class ofxOpenNIContext;
class ofxDepthGenerator;
class ofxOpenNIPlayback {
public:
	ofxOpenNIPlayback();
	void setup(ofxOpenNIContext* pContext);
	void play(std::string sFileRecording);
	void getDepthGenerator(ofxDepthGenerator* pDepthGenerator);

private:
	ofxOpenNIContext* context;
};