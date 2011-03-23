#ifndef _H_OFXDEPTHGENERATOR
#define _H_OFXDEPTHGENERATOR

#include "ofxOpenNIContext.h"

class ofxDepthGenerator {
	
public:
	
	ofxDepthGenerator();
	
	bool				setup(ofxOpenNIContext* pContext);
	
	void				draw(float x=0, float y=0, float w=640, float h=480);
	void				update();
	
	unsigned char*		getDepthPixels(int nearThreshold = 0, int farThreshold = 10000);
	
	xn::DepthGenerator&	getXnDepthGenerator();
	
	int					getMaxDepth();
	int					getWidth();
	int					getHeight();
	
private:
	
	void				generateTexture();
	
	xn::DepthGenerator	depth_generator;
	ofTexture			depth_texture;
	unsigned char *		depth_pixels;
	int					depth_coloring;
	
	int					width, height;
	int					max_depth;
	
	unsigned char*		maskPixels;
};

#endif