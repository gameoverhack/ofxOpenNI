#ifndef USERGENERATORH
#define USERGENERATORH

#include "ofxOpenNIContext.h"

class ofxUserGenerator {
public:
	ofxUserGenerator();
	bool setup(ofxOpenNIContext* pContext);
private:
	xn::UserGenerator user_generator;
	ofxOpenNIContext* context;
};
#endif

/*
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
*/