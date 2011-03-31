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
	depth_coloring = COLORING_RAINBOW;
}

bool ofxDepthGenerator::setup(ofxOpenNIContext* pContext) {

	XnStatus result = XN_STATUS_OK;	
	XnMapOutputMode map_mode; 
	
	// Try to fetch depth generator before creating one
	if(pContext->getDepthGenerator(&depth_generator)) {
		// found the depth generator so set map_mode from it
		depth_generator.GetMapOutputMode(map_mode);
	} else {
		result = depth_generator.Create(pContext->getXnContext());
		CHECK_RC(result, "Creating depth generator");
		
		// make new map mode -> default to 640 x 480 @ 30fps
		map_mode.nXRes = XN_VGA_X_RES;
		map_mode.nYRes = XN_VGA_Y_RES;
		map_mode.nFPS  = 30;
		
		depth_generator.SetMapOutputMode(map_mode);
	}	

	// Default max depth is on GlobalDefaults.ini: MaxDepthValue=10000
	max_depth	= depth_generator.GetDeviceMaxDepth();		
	width		= map_mode.nXRes;
	height		= map_mode.nYRes;
	
	// TODO: add capability for b+w depth maps (more efficient for draw)
	depth_texture.allocate(map_mode.nXRes, map_mode.nYRes, GL_RGBA);		
	depth_pixels = new unsigned char[map_mode.nXRes * map_mode.nYRes * 4];
	memset(depth_pixels, 0, map_mode.nXRes * map_mode.nYRes * 4 * sizeof(unsigned char));
	
	depth_generator.StartGenerating();	
	
	// setup mask pixels TODO: make do multiple ranges
	maskPixels = new unsigned char[map_mode.nXRes * map_mode.nYRes];
	
	printf("Depth camera inited");
	
	return true;
}
void ofxDepthGenerator::update() {
	// get meta-data
	depth_generator.GetMetaData(dmd);
	this->generateTexture();
}

void ofxDepthGenerator::draw(float x, float y, float w, float h) {
	glColor3f(1,1,1);
	depth_texture.draw(x, y, w, h);	
}

void ofxDepthGenerator::setDepthColoring(enumDepthColoring c) {
	depth_coloring = c;
}


// returns mask pixels in a range TODO: make do multiple ranges
unsigned char* ofxDepthGenerator::getDepthPixels(int nearThreshold, int farThreshold) {
	
	xn::DepthMetaData dmd;
	depth_generator.GetMetaData(dmd);
	const XnDepthPixel* depth = dmd.Data();
	
	int numPixels = dmd.XRes() * dmd.YRes();
	for(int i = 0; i < numPixels; i++, depth++) {
		if(*depth < farThreshold && *depth > nearThreshold) {
			maskPixels[i] = 255;
		} else {
			maskPixels[i] = 0;
		}
	}
	return maskPixels;
}

int ofxDepthGenerator::getWidth() {
	return width;
}

int ofxDepthGenerator::getHeight() {
	return height;
}

int ofxDepthGenerator::getMaxDepth() {
	return max_depth;
}

xn::DepthGenerator& ofxDepthGenerator::getXnDepthGenerator() {
	return depth_generator;
}

// Get a pixel from the depth map
ofColor ofxDepthGenerator::getPixel( const ofPoint & p )
{
	return this->getPixel( (int)p.x, (int)p.y );
}
ofColor ofxDepthGenerator::getPixel( int x, int y )
{
	int i = ( y * XN_VGA_X_RES + x ) * 4;
	return ofColor( depth_pixels[i], depth_pixels[i+1], depth_pixels[i+2], depth_pixels[i+3] );
}



// TODO: add capability for b+w depth maps (more efficient for draw)
void ofxDepthGenerator::generateTexture(){
	
	// get the pixels
	const XnDepthPixel* depth = dmd.Data();
	XN_ASSERT(depth);
	
	if (dmd.FrameID() == 0) return;

	// copy depth into texture-map
	float max;
	for (XnUInt16 y = dmd.YOffset(); y < dmd.YRes() + dmd.YOffset(); y++) {
		unsigned char * texture = (unsigned char*)depth_pixels + y * dmd.XRes() * 4 + dmd.XOffset() * 4;
		for (XnUInt16 x = 0; x < dmd.XRes(); x++, depth++, texture += 4) {
			XnUInt8 red = 0;
			XnUInt8 green = 0;
			XnUInt8 blue = 0;
			XnUInt8 alpha = 255;
			
			XnUInt16 col_index;
			
			switch (depth_coloring){
				case COLORING_PSYCHEDELIC_SHADES:
					alpha *= (((XnFloat)(*depth % 10) / 20) + 0.5);
				case COLORING_PSYCHEDELIC:
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
				case COLORING_RAINBOW:
					col_index = (XnUInt16)(((*depth) / (max_depth / 256)));
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case COLORING_CYCLIC_RAINBOW:
					col_index = (*depth % 256);
					red = PalletIntsR[col_index];
					green = PalletIntsG[col_index];
					blue = PalletIntsB[col_index];
					break;
				case COLORING_BLUES:
					// 3 bytes of depth: black (R0G0B0) >> blue (001) >> cyan (011) >> white (111)
					max = 256+255+255;
					col_index = (XnUInt16)(((*depth) / ( max_depth / max)));
					if ( col_index < 256 )
					{
						blue	= col_index;
						green	= 0;
						red		= 0;
					}
					else if ( col_index < (256+255) )
					{
						blue	= 255;
						green	= (col_index % 256) + 1;
						red		= 0;
					}
					else if ( col_index < (256+255+255) )
					{
						blue	= 255;
						green	= 255;
						red		= (col_index % 256) + 1;
					}
					else
					{
						blue	= 255;
						green	= 255;
						red		= 255;
					}
					break;
				case COLORING_GREY:
					max = 255;	// half depth
					{
						XnUInt8 a = (XnUInt8)(((*depth) / ( max_depth / max)));
						red		= a;
						green	= a;
						blue	= a;
					}
					break;
				case COLORING_STATUS:
					// This is something to use on installations
					// when the end user needs to know if the camera is tracking or not
					// The scene will be painted GREEN if status == true
					// The scene will be painted RED if status == false
					// Usage: declare a global bool status and that's it!
					// I'll keep it commented so you dont have to have a status on every project
#if 0
					{
						extern bool status;
						max = 255;	// half depth
						XnUInt8 a = 255 - (XnUInt8)(((*depth) / ( max_depth / max)));
						red		= ( status ? 0 : a);
						green	= ( status ? a : 0);
						blue	= 0;
					}
#endif
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
	
	depth_texture.loadData((unsigned char *)depth_pixels, dmd.XRes(), dmd.YRes(), GL_RGBA);	
}



