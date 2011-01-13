#include "ofxImageGenerator.h"
 
void ofxImageGenerator::generateTexture(){
	xn::ImageMetaData imd;
	image_generator.GetMetaData(imd);	
	const XnUInt8* pImage = imd.Data();
	memcpy(image_pixels, pImage, sizeof(unsigned char) * imd.XRes() * imd.YRes() * 3);
	image_texture.loadData(image_pixels,imd.XRes(), imd.YRes(), GL_RGB);		
}

void ofxImageGenerator::draw(float x, float y, float w, float h){
	generateTexture();
	glColor3f(1,1,1);
	image_texture.draw(x, y, w, h);		
}

bool ofxImageGenerator::setup(ofxOpenNIContext* pContext) {
	if(!pContext->isInitialized()) {
		return false;
	}
	
	//Create image generator
	XnStatus result = image_generator.Create(pContext->getXnContext());
	
	if (result != XN_STATUS_OK){
		printf("Setup Image Camera failed: %s\n", xnGetStatusString(result));
		return false;
	} else {
		ofLog(OF_LOG_VERBOSE, "Create Image Generator inited");
		
		XnMapOutputMode map_mode; 
		map_mode.nXRes = XN_VGA_X_RES; 
		map_mode.nYRes = XN_VGA_Y_RES; 
		map_mode.nFPS = 30;
		
		result = image_generator.SetMapOutputMode(map_mode);
		
		image_texture.allocate(map_mode.nXRes, map_mode.nYRes, GL_RGBA);		
		image_pixels = new unsigned char[map_mode.nXRes * map_mode.nYRes * 3];
		
		image_generator.StartGenerating();		
		return true;
	}		
}

xn::ImageGenerator& ofxImageGenerator::getXnImageGenerator(){
	return image_generator;
}
