#ifndef _H_OFXIMAGEENERATOR
#define _H_OFXIMAGEENERATOR

#include "ofxOpenNIContext.h"

class ofxImageGenerator {
	
public:
	
	ofxImageGenerator();
	~ofxImageGenerator();
	
	bool setup(ofxOpenNIContext* pContext);
	void update();
	void draw(float x=0, float y=0, float w=640, float h=480);
	
	unsigned char*		getPixels() {return image_pixels;};
	
	xn::ImageGenerator& getXnImageGenerator();
	
private:
	
	void generateTexture();
	
	xn::ImageGenerator	image_generator;
	ofTexture			image_texture;
	unsigned char *		image_pixels;
	
	ofxImageGenerator(ofxImageGenerator const& other);
	ofxImageGenerator & operator = (const ofxImageGenerator&);
	
};

#endif