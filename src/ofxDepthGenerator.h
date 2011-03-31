#pragma once

#include "ofxOpenNIContext.h"
#include "ofxImageGenerator.h"

enum enumDepthColoring {
	COLORING_PSYCHEDELIC_SHADES = 0,
	COLORING_PSYCHEDELIC,
	COLORING_RAINBOW,
	COLORING_CYCLIC_RAINBOW,
	COLORING_BLUES,
	COLORING_GREY,
	COLORING_STATUS,
	COLORING_COUNT
};


class ofxDepthGenerator {
public:
	ofxDepthGenerator();
	
	bool setup(ofxOpenNIContext* pContext);
	
	void update();
	
	void applyDepthCutoff();
	
	void draw(float x=0, float y=0, float w=640, float h=480);
	
	bool toggleRegisterViewport(ofxImageGenerator* image_generator);
	
	void generateTexture();
	
	// ROGER
	void setDepthColoring(enumDepthColoring c)	{ depth_coloring = c; };
	float getMaxDepth()							{ return max_depth; };
	ofColor getPixel(int x, int y);
	ofColor getPixel(const ofPoint & p);
	
	
	xn::DepthGenerator& getXnDepthGenerator();
	
private:
	
	xn::DepthGenerator depth_generator;
	xn::DepthMetaData dmd;

	ofTexture depth_texture;
	unsigned char * depth_pixels;
	int depth_coloring;
	float max_depth;
};

