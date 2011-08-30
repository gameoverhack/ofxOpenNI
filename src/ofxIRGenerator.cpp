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
	const XnUInt8* pImage = (XnUInt8*)ird.Data();

	int j = 0;
	
	for (int i = 0; i < ird.XRes() * ird.YRes()*2; i+=2, j++) {		// Don't ask me why ;-)
		ir_pixels[j] = pImage[i];
	}
	
	//memcpy(ir_pixels, pImage, sizeof(unsigned char) * ird.XRes() * ird.YRes());
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
