#pragma once

#include "ofxOpenNIContext.h"

class ofxIRGenerator {
public:
	
	ofxIRGenerator(){};
	
	bool setup(ofxOpenNIContext* pContext);
	void draw(float x=0, float y=0, float w=640, float h=480);
	
	xn::IRGenerator& getXnIRGenerator();
	
private:
	
	void generateTexture();
	
	xn::IRGenerator		ir_generator;
	ofTexture			ir_texture;
	unsigned char *		ir_pixels;
	
};

