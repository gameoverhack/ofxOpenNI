#ifndef _H_OFXIRGENERATOR
#define _H_OFXIRGENERATOR

#include "ofxOpenNIContext.h"

class ofxIRGenerator {
	
public:
	
	ofxIRGenerator();
	~ofxIRGenerator();
	
	bool setup(ofxOpenNIContext* pContext);
	void update();
	void draw(float x=0, float y=0, float w=640, float h=480);
	
	xn::IRGenerator& getXnIRGenerator();
	
//	ofxIRGenerator(ofxIRGenerator const& mom);
//	ofxIRGenerator & operator = (const ofxIRGenerator& mom);
	
private:
	
	void generateTexture();
	
	xn::IRGenerator		ir_generator;
	ofTexture			ir_texture;
	unsigned char *		ir_pixels;
	
};

#endif