#ifndef OFXDEPTHGENERATORH
#define OFXDEPTHGENERATORH

#include "ofxOpenNIContext.h"


class ofxDepthGenerator {
public:
	ofxDepthGenerator();
	
	bool setup(ofxOpenNIContext * context);
	void generateTexture();
	void draw(float x=0, float y=0, float w=640, float h=480);

private:
	xn::DepthGenerator g_DepthGenerator;
	ofTexture _depthTexture;
	unsigned char * depthPixels;
	int _depthColoring;
	float g_fMaxDepth;

	ofxOpenNIContext * _context;
};

#endif