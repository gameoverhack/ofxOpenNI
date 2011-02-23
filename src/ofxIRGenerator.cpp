#include "ofxIRGenerator.h"

void ofxIRGenerator::generateTexture() {
	
	xn::IRMetaData ird;
	ir_generator.GetMetaData(ird);	
	const XnUInt8* pImage = (XnUInt8*)ird.Data();
	
	memcpy(ir_pixels, pImage, sizeof(unsigned char) * ird.XRes() * ird.YRes());
	ir_texture.loadData(ir_pixels, ird.XRes(), ird.YRes(), GL_LUMINANCE);	
	
}

void ofxIRGenerator::draw(float x, float y, float w, float h){
	generateTexture();
	glColor3f(1,1,1);
	ir_texture.draw(x, y, w, h);		
}

bool ofxIRGenerator::setup(ofxOpenNIContext* pContext) {
	
	//Create image generator
	XnStatus result = ir_generator.Create(pContext->getXnContext());
	
	if (result != XN_STATUS_OK){
		printf("Setup Infra Red Camera failed: %s\n", xnGetStatusString(result));
		return false;
	} else {
		ofLog(OF_LOG_VERBOSE, "Create Infra Red Generator inited");
		
		XnMapOutputMode map_mode; 
		map_mode.nXRes = XN_VGA_X_RES; 
		map_mode.nYRes = XN_VGA_Y_RES; 
		map_mode.nFPS = 30;
		
		result = ir_generator.SetMapOutputMode(map_mode);
		
		ir_texture.allocate(map_mode.nXRes, map_mode.nYRes, GL_LUMINANCE);		
		ir_pixels = new unsigned char[map_mode.nXRes * map_mode.nYRes];
		
		ir_generator.StartGenerating();		
		return true;
	}
	
}

xn::IRGenerator& ofxIRGenerator::getXnIRGenerator(){
	return ir_generator;
}
