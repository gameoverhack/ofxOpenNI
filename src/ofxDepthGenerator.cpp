#include "ofxDepthGenerator.h"
#include "ofxOpenNIMacros.h"

XnUInt8 PalletIntsR [256] = {0};
XnUInt8 PalletIntsG [256] = {0};
XnUInt8 PalletIntsB [256] = {0};

void CreateRainbowPallet() {
	unsigned char r, g, b;
	for (int i=1; i<255; i++) {
		if (i<=29) {
			r = (unsigned char)(129.36-i*4.36);
			g = 0;
			b = (unsigned char)255;
		}
		else if (i<=86) {
			r = 0;
			g = (unsigned char)(-133.54+i*4.52);
			b = (unsigned char)255;
		}
		else if (i<=141) {
			r = 0;
			g = (unsigned char)255;
			b = (unsigned char)(665.83-i*4.72);
		}
		else if (i<=199) {
			r = (unsigned char)(-635.26+i*4.47);
			g = (unsigned char)255;
			b = 0;
		}
		else {
			r = (unsigned char)255;
			g = (unsigned char)(1166.81-i*4.57);
			b = 0;
		}
		PalletIntsR[i] = r;
		PalletIntsG[i] = g;
		PalletIntsB[i] = b;
	}
}

ofxDepthGenerator::ofxDepthGenerator(){
	CreateRainbowPallet();	
	depth_coloring = 2;
}

bool ofxDepthGenerator::setup(ofxOpenNIContext* pContext) {
	if(!pContext->isInitialized()) {
		return false;
	}
	
	XnStatus result = XN_STATUS_OK;	
	
	// check if the USER generator exists.
	result = pContext->getXnContext()
					.FindExistingNode(XN_NODE_TYPE_DEPTH, depth_generator);
	SHOW_RC(result, "Find depth generator");
	if(result != XN_STATUS_OK) {
		result = depth_generator.Create(pContext->getXnContext());
		SHOW_RC(result, "Create depth generator");
		if(result != XN_STATUS_OK) {			
			return false;
		}
	}	
	
	
	ofLog(OF_LOG_VERBOSE, "Depth camera inited");
	
	
	//Set the input to VGA (standard is QVGA wich is not supported on the Kinect)

	XnMapOutputMode map_mode; 
	map_mode.nXRes = XN_VGA_X_RES; 
	map_mode.nYRes = XN_VGA_Y_RES;
	map_mode.nFPS = 30;
	
	result = depth_generator.SetMapOutputMode(map_mode);
	max_depth = depth_generator.GetDeviceMaxDepth();		
	
	depth_texture.allocate(map_mode.nXRes, map_mode.nYRes, GL_RGBA);		
	depth_pixels = new unsigned char[map_mode.nXRes * map_mode.nYRes * 4];
	memset(depth_pixels, 0, map_mode.nXRes * map_mode.nYRes * 4 * sizeof(unsigned char));
	
	depth_generator.StartGenerating();	
	return true;
	
}
void ofxDepthGenerator::draw(float x, float y, float w, float h){
	generateTexture();
	glColor3f(1,1,1);
	depth_texture.draw(x, y, w, h);	
}

xn::DepthGenerator& ofxDepthGenerator::getXnDepthGenerator() {
	return depth_generator;
}

bool ofxDepthGenerator::registerViewport(ofxImageGenerator* image_generator) {
	// Register view point to image map
	
	XnStatus result;	
	
	if (depth_generator.IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT))
	{
		result = depth_generator.GetAlternativeViewPointCap().SetViewPoint(image_generator->getXnImageGenerator());
		CHECK_RC(result, "Registration");
	} else return false;
}

void ofxDepthGenerator::generateTexture(){
	// get meta-data
	xn::DepthMetaData dmd;
	depth_generator.GetMetaData(dmd);	
	
	// get the pixels
	const XnDepthPixel* depth = dmd.Data();
	XN_ASSERT(depth);
	
	if (dmd.FrameID() == 0){
		return;
	}
	if (dmd.PixelFormat() == XN_PIXEL_FORMAT_RGB24) {
		printf("its in yuv\n");
	}
	
	// copy depth into texture-map
	for (XnUInt16 y = dmd.YOffset(); y < dmd.YRes() + dmd.YOffset(); y++) {
		unsigned char * texture = (unsigned char*)depth_pixels + y * dmd.XRes() * 4 + dmd.XOffset()*4;
		for (XnUInt16 x = 0; x < dmd.XRes(); x++, depth++, texture+=4){
			XnUInt8 red = 0;
			XnUInt8 green = 0;
			XnUInt8 blue = 0;
			XnUInt8 alpha = 255;
			
			XnUInt16 col_index;
			//printf("%d\n", depth);
			
			switch (depth_coloring){
				case 0: //PSYCHEDELIC_SHADES
					alpha *= (((XnFloat)(*depth % 10) / 20) + 0.5);
				case 1: //PSYCHEDELIC					
					switch ((*depth/10) % 10){
						case 0:
							red = 255;
							break;
						case 1:
							green = 255;
							break;
						case 2:
							blue = 255;
							break;
						case 3:
							red = 255;
							green = 255;
							break;
						case 4:
							green = 255;
							blue = 255;
							break;
						case 5:
							red = 255;
							blue = 255;
							break;
						case 6:
							red = 255;
							green = 255;
							blue = 255;
							break;
						case 7:
							red = 127;
							blue = 255;
							break;
						case 8:
							red = 255;
							blue = 127;
							break;
						case 9:
							red = 127;
							green = 255;
							break;
					}
					break;
				case 2: //RAINBOW
					col_index = (XnUInt16)(((*depth) / (max_depth / 256)));
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case 3: //CYCLIC_RAINBOW
					col_index = (*depth % 256);
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
			}
			
			texture[0] = red;
			texture[1] = green;
			texture[2] = blue;
			
			if (*depth == 0)
				texture[3] = 0;
			else
				texture[3] = alpha;
		}	
	}
	
	depth_texture.loadData((unsigned char *)depth_pixels,dmd.XRes(), dmd.YRes(), GL_RGBA);	
}