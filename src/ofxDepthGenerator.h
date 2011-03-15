#ifndef __H_OFXDEPTHGENERATOR
#define __H_OFXDEPTHGENERATOR

#include "ofxOpenNIContext.h"
//#include "ofxImageGenerator.h"
class ofxDepthGenerator {
	
public:
	
	ofxDepthGenerator();
	
	bool setup(ofxOpenNIContext* pContext);
	
	void draw(float x=0, float y=0, float w=640, float h=480);
	void update();
	
	xn::DepthGenerator&	getXnDepthGenerator();
	
private:
	
	void generateTexture();
	
	xn::DepthGenerator	depth_generator;
	ofTexture			depth_texture;
	unsigned char *		depth_pixels;
	int					depth_coloring;
	float				max_depth;
	
};

#endif