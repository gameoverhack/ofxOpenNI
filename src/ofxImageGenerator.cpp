/*
 * ofxImageGenerator.cpp
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

#include "ofxImageGenerator.h"
#include "ofxOpenNIMacros.h"

ofxImageGenerator::ofxImageGenerator(){
	//nothing
}
ofxImageGenerator::~ofxImageGenerator(){
	//nothing
}

void ofxImageGenerator::generateTexture() {
	
	xn::ImageMetaData imd;
	image_generator.GetMetaData(imd);	
	const XnUInt8* pImage = imd.Data();
	
	memcpy(image_pixels, pImage, sizeof(unsigned char) * imd.XRes() * imd.YRes() * 3);
	image_texture.loadData(image_pixels,imd.XRes(), imd.YRes(), GL_RGB);		
}

void ofxImageGenerator::update() {
	generateTexture();
}

void ofxImageGenerator::draw(float x, float y, float w, float h){
	glColor3f(1,1,1);
	image_texture.draw(x, y, w, h);		
}

bool ofxImageGenerator::setup(ofxOpenNIContext* pContext) {
	
	XnStatus result = XN_STATUS_OK;	
	XnMapOutputMode map_mode;
	
	// check we don't already have an IR generator -> can only have an image OR an ir gen
	xn::IRGenerator ir_generator;
	if(pContext->getIRGenerator(&ir_generator)) {
		printf("Can't init image generator: can only have image OR IR gen, not both!!!");
		return false;
	}
	
	// Try to fetch image generator before creating one
	if(pContext->getImageGenerator(&image_generator)) {
		// found the image generator so set map_mode from it
		image_generator.GetMapOutputMode(map_mode);
	} else {
		result = image_generator.Create(pContext->getXnContext());
		CHECK_RC(result, "Creating image generator");
		
		if (result != XN_STATUS_OK) return false;
		
		// make new map mode -> default to 640 x 480 @ 30fps
		map_mode.nXRes = XN_VGA_X_RES;
		map_mode.nYRes = XN_VGA_Y_RES;
		map_mode.nFPS  = 30;
		
		image_generator.SetMapOutputMode(map_mode);
	}

	// TODO: add capability for b+w depth maps (more efficient for draw)
	image_texture.allocate(map_mode.nXRes, map_mode.nYRes, GL_RGB);		
	image_pixels = new unsigned char[map_mode.nXRes * map_mode.nYRes * 3];
	memset(image_pixels, 0, map_mode.nXRes * map_mode.nYRes * 3 * sizeof(unsigned char));
	
	image_generator.StartGenerating();	
	
	printf("Image camera inited\n");
	
	return true;
	
}

xn::ImageGenerator& ofxImageGenerator::getXnImageGenerator() {
	return image_generator;
}