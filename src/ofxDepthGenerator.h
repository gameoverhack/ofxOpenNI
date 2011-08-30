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

typedef struct {
	int nearThreshold;
	int farThreshold;
} depth_threshold;

// this must be at least 1!
#define MAX_NUMBER_DEPTHS 4

class ofxDepthGenerator {
	
public:
	
	ofxDepthGenerator();
	
	bool				setup(ofxOpenNIContext* pContext);
	
	void				draw(float x=0, float y=0, float w=640, float h=480);
	void				update();
	
	void				setMaxNumDepthThresholds(int num);
	int					getMaxNumDepthThresholds() {return max_number_depths;};
	void				setDepthThreshold(int forDepthThresholdNumber = 0, int nearThreshold = 0, int farThreshold = 10000);
	
	unsigned char*		getDepthPixels(int forDepthThresholdNumber);
	unsigned char*		getDepthPixels(int nearThreshold, int farThreshold);
	
	xn::DepthGenerator&	getXnDepthGenerator();
	
	void				setDepthColoring(enumDepthColoring c);
	
	int					getMaxDepth();
	int					getWidth();
	int					getHeight();

	ofColor				getPixelColor(int x, int y);
	ofColor				getPixelColor(const ofPoint & p);
	int					getPixelDepth(int x, int y);
	
private:
	
	void				generateTexture();

	void				updateMaskPixels();
	
	xn::DepthGenerator	depth_generator;
	xn::DepthMetaData	dmd;

	ofTexture			depth_texture;
	unsigned char*		depth_pixels;
	int					depth_coloring;
	int					width, height;
	float				max_depth;
	unsigned char*		maskPixels[MAX_NUMBER_DEPTHS];
	depth_threshold		depth_thresholds[MAX_NUMBER_DEPTHS];
	int					max_number_depths;
	
	ofxDepthGenerator(const ofxDepthGenerator& other);
	ofxDepthGenerator& operator = (const ofxDepthGenerator&);
	
};

#endif
