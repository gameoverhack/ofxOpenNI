#ifndef _H_OFXDEPTHGENERATOR
#define _H_OFXDEPTHGENERATOR

#include "ofxOpenNIContext.h"

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
	
	bool				setup(ofxOpenNIContext* pContext);
	
	void				draw(float x=0, float y=0, float w=640, float h=480);
	void				update();
	
	unsigned char*		getDepthPixels(int nearThreshold = 0, int farThreshold = 10000);
	
	xn::DepthGenerator&	getXnDepthGenerator();
	
	void				setDepthColoring(enumDepthColoring c);
	
	int					getMaxDepth();
	int					getWidth();
	int					getHeight();

	ofColor				getPixel(int x, int y);
	ofColor				getPixel(const ofPoint & p);

private:
	
	void				generateTexture();

	xn::DepthGenerator	depth_generator;
	xn::DepthMetaData	dmd;

	ofTexture			depth_texture;
	unsigned char *		depth_pixels;
	int					depth_coloring;
	int					width, height;
	float				max_depth;
	unsigned char*		maskPixels;
};

#endif
