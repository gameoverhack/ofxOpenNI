/*
 * ofxDepthGenerator.h
 *
 * Copyright 2011 (c) Matthew Gingold http://gingold.com.au
 * Originally forked from a project by roxlu http://www.roxlu.com/ 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

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
