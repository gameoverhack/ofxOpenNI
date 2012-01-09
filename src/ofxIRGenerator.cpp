/*
 * ofxIRGenerator.cpp
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

#include "ofxIRGenerator.h"
#include "ofxOpenNIMacros.h"

ofxIRGenerator::ofxIRGenerator(){
	//nothing
}
ofxIRGenerator::~ofxIRGenerator(){
	//nothing
}

void ofxIRGenerator::generateTexture() {
	
	xn::IRMetaData ird;
	ir_generator.GetMetaData(ird);	
	const XnIRPixel* pImage = ird.Data();

	for (int i = 0; i < ird.XRes() * ird.YRes(); i++) {
		ir_pixels[i] = pImage[i]/4;
	}
		
	ir_texture.loadData(ir_pixels, ird.XRes(), ird.YRes(), GL_LUMINANCE);	
	
}

void ofxIRGenerator::update() {
	generateTexture();
}

void ofxIRGenerator::draw(float x, float y, float w, float h){
	glColor3f(1,1,1);
	ir_texture.draw(x, y, w, h);		
}

bool ofxIRGenerator::setup(ofxOpenNIContext* pContext) {
	
	XnStatus result = XN_STATUS_OK;	
	XnMapOutputMode map_mode;
	
	// check we don't already have an image generator -> can only have an image OR an ir gen
	xn::ImageGenerator image_generator;
	if(pContext->getImageGenerator(&image_generator)) {
		printf("Can't init IR generator: can only have image OR IR gen, not both!!!");
		return false;
	}
	
	// Try to fetch ir generator before creating one
	if(pContext->getIRGenerator(&ir_generator)) {
		// found the ir generator so set map_mode from it
		ir_generator.GetMapOutputMode(map_mode);
	} else {
		result = ir_generator.Create(pContext->getXnContext());
		CHECK_RC(result, "Creating IR generator");
		
		if (result != XN_STATUS_OK) return false;
		
		// make new map mode -> default to 640 x 480 @ 30fps
		map_mode.nXRes = XN_VGA_X_RES;
		map_mode.nYRes = XN_VGA_Y_RES;
		map_mode.nFPS  = 30;
		
		ir_generator.SetMapOutputMode(map_mode);
	}
	
	ir_texture.allocate(map_mode.nXRes, map_mode.nYRes, GL_LUMINANCE);		
	ir_pixels = new unsigned char[map_mode.nXRes * map_mode.nYRes ];
	memset(ir_pixels, 0, map_mode.nXRes * map_mode.nYRes * sizeof(unsigned char));
	
	ir_generator.StartGenerating();		
	
	printf("IR camera inited\n");
	
	return true;
	
}

xn::IRGenerator& ofxIRGenerator::getXnIRGenerator(){
	return ir_generator;
}
