/*
 * ofxOpenNI.cpp
 *
 * Copyright 2011 (c) Matthew Gingold [gameover] http://gingold.com.au
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

#include "ofxOpenNI.h"

string ofxOpenNI::LOG_NAME = "ofxOpenNI";
static bool rainbowPalletInit = false;
static int instanceCount = -1;

XnUInt8 PalletIntsR [256] = {0};
XnUInt8 PalletIntsG [256] = {0};
XnUInt8 PalletIntsB [256] = {0};

//--------------------------------------------------------------
static void CreateRainbowPallet() {
	if (rainbowPalletInit) return;
	
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
	rainbowPalletInit = true;
}

//--------------------------------------------------------------
ofxOpenNI::ofxOpenNI() {
	
	instanceCount++;
	instanceID = instanceCount; // TODO: this should be replaced/combined with a listDevices and setDeviceID methods
	
	numDevices = 0;
	bIsThreaded = false;
	
	g_bIsDepthOn = false;
	g_bIsDepthRawOnOption = false;
	g_bIsImageOn = false;
	g_bIsIROn = false;
	g_bIsAudioOn = false;
	g_bIsPlayerOn = false;
	
	depthColoring = COLORING_RAINBOW;
	
	bUseTexture = true;
	bNewPixels = false;
	bNewFrame = false;	
	
	CreateRainbowPallet();
	
}

//--------------------------------------------------------------
ofxOpenNI::~ofxOpenNI() {
	ofLogVerbose(LOG_NAME) << "Shuting down device:" << instanceID;
	if (bIsThreaded) {
		lock();
		stopThread();
	}
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(bool threaded) {

	XnStatus nRetVal;
	
	//	nRetVal = g_Context.StopGeneratingAll();
	//	ofLogVerbose(LOG_NAME) << "Stop ALL generators:" << xnGetStatusString(nRetVal);
	
	bIsThreaded = threaded;
	
	if (!openNIContext->getIsContextReady()) {
		if (!openNIContext->initContext()) {
			ofLogError(LOG_NAME) << "Context could not be intitilised";
			return false;
		}
	}
	
	//initDevice();
	
	if (openNIContext->getNumDevices() > 0) {
		
		ofLogNotice(LOG_NAME) << "Found" << openNIContext->getNumDevices() << "devices.";
		
		//		nRetVal = g_Context.StartGeneratingAll();
		//		ofLogVerbose(LOG_NAME) << "Start ALL generators:" << xnGetStatusString(nRetVal);
		
		//startThread(false, false);
		startThread(true, false);
		
		return true;
	} else {
		ofLogNotice(LOG_NAME) << "No Devices found!"; //Set up player...??
		//		numDevices = enumerateAndCreate(XN_NODE_TYPE_PLAYER, g_Player);
		//		if (numDevices > 0) {
		//			if (bIsThreaded) startThread(false, false);
		//			nRetVal = g_Context.StartGeneratingAll();
		//			ofLogVerbose(LOG_NAME) << "Start ALL generators:" << xnGetStatusString(nRetVal);
		//			
		//			return true;
		//		} else {
		//			ofLogError(LOG_NAME) << "Could not setup Players!";
		//			return false;
		//		}
	}
	
}

//--------------------------------------------------------------
bool ofxOpenNI::addDepthGenerator() {
	XnStatus nRetVal;
	ofLogVerbose(LOG_NAME) << "Add depth generator...";
	if (!openNIContext->getIsContextReady()) {
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
		g_bIsDepthOn = false;
	}
	if (openNIContext->getDepthGenerator(instanceID).IsValid()) {
		g_bIsDepthOn = true;
		allocateDepthBuffers();
		nRetVal = openNIContext->getDepthGenerator(instanceID).StartGenerating();
		ofLogVerbose(LOG_NAME) << "Starting depth generator" << xnGetStatusString(nRetVal);
	} else {
		ofLogError(LOG_NAME) << "Depth generator is invalid!";
		g_bIsDepthOn = false;
	}
	return g_bIsDepthOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addImageGenerator() {
	XnStatus nRetVal;
	ofLogVerbose(LOG_NAME) << "Add image generator...";
	if (!openNIContext->getIsContextReady()) {
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
		g_bIsImageOn = false;
	}
	if (openNIContext->getImageGenerator(instanceID).IsValid()) {
		g_bIsImageOn = true;
		allocateImageBuffers();
		nRetVal = openNIContext->getImageGenerator(instanceID).StartGenerating();
		ofLogVerbose() << "Starting image generator" << xnGetStatusString(nRetVal);
	} else {
		ofLogError(LOG_NAME) << "Image generator is invalid!";
		g_bIsImageOn = false;
	}
	return g_bIsImageOn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addInfraGenerator() {
	XnStatus nRetVal;
	ofLogVerbose(LOG_NAME) << "Add ir generator...";
	ofLogWarning(LOG_NAME) << "IR GENERATOR IS NOT QUITE WORKING YET!";
	if (!openNIContext->getIsContextReady()) {
		ofLogError(LOG_NAME) << "Context is not setup - please call ofxOpenNI::setup() first";
		g_bIsIROn = false;
	}
	if (openNIContext->getIRGenerator(instanceID).IsValid()) {
		g_bIsIROn = true;
		allocateIRBuffers();
		nRetVal = openNIContext->getIRGenerator(instanceID).StartGenerating();
		ofLogVerbose() << "Starting ir generator" << xnGetStatusString(nRetVal);
	} else {
		ofLogError(LOG_NAME) << "Image generator is invalid!";
		g_bIsIROn = false;
	}
	return g_bIsIROn;
}

//--------------------------------------------------------------
bool ofxOpenNI::addAudioGenerator() {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
bool ofxOpenNI::addUserGenerator() {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
bool ofxOpenNI::addPlayerGenerator() {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
void ofxOpenNI::allocateDepthBuffers() {
	if (g_bIsDepthOn) {
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = openNIContext->getDepthGenerator(instanceID).SetMapOutputMode(mapMode);
		maxDepth = openNIContext->getDepthGenerator(instanceID).GetDeviceMaxDepth();
		ofLogVerbose(LOG_NAME) << "Setting depth resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << maxDepth << mapMode.nFPS;
		depthPixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR_ALPHA);
		depthPixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR_ALPHA);
		currentDepthPixels = &depthPixels[0];
		backDepthPixels = &depthPixels[1];
		if (bUseTexture) depthTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_RGBA);
	}
}

//--------------------------------------------------------------
void ofxOpenNI::allocateDepthRawBuffers() {
	if (g_bIsDepthRawOnOption) {
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = openNIContext->getDepthGenerator(instanceID).SetMapOutputMode(mapMode);
		maxDepth = openNIContext->getDepthGenerator(instanceID).GetDeviceMaxDepth();
		ofLogVerbose(LOG_NAME) << "Setting depth resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << maxDepth << mapMode.nFPS;
		depthRawPixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_PIXELS_MONO);
		depthRawPixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_PIXELS_MONO);
		currentDepthRawPixels = &depthRawPixels[0];
		backDepthRawPixels = &depthRawPixels[1];
	}
}

//--------------------------------------------------------------
void ofxOpenNI::allocateImageBuffers() {
	if (g_bIsImageOn) {
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = openNIContext->getImageGenerator(instanceID).SetMapOutputMode(mapMode);
		ofLogVerbose(LOG_NAME) << "Setting image resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << mapMode.nFPS;
		imagePixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR);
		imagePixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_COLOR);
		currentImagePixels = &imagePixels[0];
		backImagePixels = &imagePixels[1];
		if (bUseTexture) imageTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_RGB);
	}
}

//--------------------------------------------------------------
void ofxOpenNI::allocateIRBuffers() {
	if (g_bIsIROn) {
		// make new map mode -> default to 640 x 480 @ 30fps
		// TODO: shift this to a setSize or make part of int/setup request
		XnMapOutputMode mapMode;
		XnStatus nRetVal;
		mapMode.nXRes = XN_VGA_X_RES;
		mapMode.nYRes = XN_VGA_Y_RES;
		mapMode.nFPS  = 30;
		nRetVal = openNIContext->getIRGenerator(instanceID).SetMapOutputMode(mapMode);
		ofLogVerbose(LOG_NAME) << "Setting ir resolution:" << xnGetStatusString(nRetVal) << mapMode.nXRes << mapMode.nYRes << mapMode.nFPS;
		imagePixels[0].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_GRAYSCALE);
		imagePixels[1].allocate(mapMode.nXRes, mapMode.nYRes, OF_IMAGE_GRAYSCALE);
		currentImagePixels = &imagePixels[0];
		backImagePixels = &imagePixels[1];
		if (bUseTexture) imageTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_LUMINANCE);
	}
}

//--------------------------------------------------------------
bool ofxOpenNI::setup(string xmlFilePath, bool threaded) {
	ofLogWarning(LOG_NAME) << "Not yet implimented";
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth() {
	if (bUseTexture) drawDepth(0, 0, g_DepthMD.XRes(), g_DepthMD.YRes());
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(float x, float y) {
	if (bUseTexture) drawDepth(x, y, g_DepthMD.XRes(), g_DepthMD.YRes());
}

//--------------------------------------------------------------
void ofxOpenNI::drawDepth(float x, float y, float w, float h) {
	if (bUseTexture) depthTexture.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage() {
	if (bUseTexture) drawImage(0, 0, g_ImageMD.XRes(), g_ImageMD.YRes());
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage(float x, float y) {
	if (bUseTexture) drawImage(x, y, g_ImageMD.XRes(), g_ImageMD.YRes());
}

//--------------------------------------------------------------
void ofxOpenNI::drawImage(float x, float y, float w, float h) {
	if (bUseTexture) imageTexture.draw(x, y, w, h);
}

//--------------------------------------------------------------
void ofxOpenNI::setDepthColoring(DepthColoring coloring) {
	depthColoring = coloring;
}

//--------------------------------------------------------------
void ofxOpenNI::readFrame() {
	
	lock(); // with this here I get 350-460 fps with 2 Kinects
	
	if (openNIContext->getDepthGenerator(instanceID).IsValid() && g_bIsDepthOn) {
		openNIContext->getDepthGenerator(instanceID).GetMetaData(g_DepthMD);
		generateDepthPixels();
	}
	
	if (openNIContext->getImageGenerator(instanceID).IsValid() && g_bIsImageOn) {
		openNIContext->getImageGenerator(instanceID).GetMetaData(g_ImageMD);
		generateImagePixels();
	}
	
	if (openNIContext->getIRGenerator(instanceID).IsValid() && g_bIsIROn) {
		openNIContext->getIRGenerator(instanceID).GetMetaData(g_IrMD);
		generateIRPixels();
	}
	
//	if (openNIContext->getAudioGenerator(instanceID).IsValid() && g_bIsAudioOn) {
//		openNIContext->getAudioGenerator(instanceID).GetMetaData(g_AudioMD);
//	}

//	lock();
//	
//	if (g_bIsDepthOn) generateDepthPixels();
//	if (g_bIsImageOn) generateImagePixels();
//	if (g_bIsIROn) generateIRPixels();
	
// I really don't think it's necessary to back buffer the image/ir/depth pixels
// as I understand it the GetMetaData() call is already acting as a back buffer
// since it is fetching the pixel data from the xn::Context which we then 'copy'
// during our generateDepth/Image/IRPixels() methods...
	
//	if (g_bIsDepthOn) {
//		swap(backDepthPixels, currentDepthPixels);
//		if (g_bIsDepthRawOnOption) {
//			swap(backDepthRawPixels, currentDepthRawPixels);
//		}
//	}
//	if (g_bIsImageOn || g_bIsIROn) {
//		swap(backImagePixels, currentImagePixels);
//	}
	
	bNewPixels = true;
	
	unlock();
}

//--------------------------------------------------------------
bool ofxOpenNI::isNewFrame() {
	return bNewFrame;
}

void ofxOpenNI::threadedFunction() {
	while(isThreadRunning()) {
		readFrame();
	}
}

//--------------------------------------------------------------
void ofxOpenNI::update() {
	if (!bIsThreaded) {
		readFrame();
	} else {
		//lock(); // if I uncomment this I get ~120-180 fps with 2 Kinects 
	}
	
	if (bNewPixels) {
		if (bUseTexture && g_bIsDepthOn) {
			//depthTexture.loadData(*currentDepthPixels);
			depthTexture.loadData(*backDepthPixels);		// do we need to lock and unlock here?
		}
		if (bUseTexture && (g_bIsImageOn || g_bIsIROn)) {
			//imageTexture.loadData(*currentImagePixels);
			imageTexture.loadData(*backImagePixels);
		}
		if (bIsThreaded) lock(); // is this worthwhile or not?
		bNewPixels = false;
		bNewFrame = true;
		if (bIsThreaded) unlock();
	}
	
	
	if (bIsThreaded) {
		//unlock();
	}
}

//--------------------------------------------------------------
bool ofxOpenNI::toggleCalibratedRGBDepth() {
	
	if (!openNIContext->getImageGenerator(instanceID).IsValid()) {
		ofLogError(LOG_NAME) << "No Image generator found: cannot register viewport";
		return false;
	}
	
	// Toggle registering view point to image map
	if (openNIContext->getDepthGenerator(instanceID).IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		
		if (openNIContext->getDepthGenerator(instanceID).GetAlternativeViewPointCap().IsViewPointAs(openNIContext->getImageGenerator(instanceID))) {
			disableCalibratedRGBDepth();
		} else {
			enableCalibratedRGBDepth();
		}
		
	} else return false;
	
	return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::enableCalibratedRGBDepth() {
	if (!openNIContext->getImageGenerator(instanceID).IsValid()) {
		ofLogError(LOG_NAME) << "No Image generator found: cannot register viewport";
		return false;
	}
	
	// Register view point to image map
	if (openNIContext->getDepthGenerator(instanceID).IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		
		XnStatus nRetVal = openNIContext->getDepthGenerator(instanceID).GetAlternativeViewPointCap().SetViewPoint(openNIContext->getImageGenerator(instanceID));
		ofLogVerbose(LOG_NAME) << "Register viewpoint depth to RGB:" << xnGetStatusString(nRetVal);
		if (nRetVal!=XN_STATUS_OK) return false;
	} else {
		ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
		return false;
	}
	
	return true;
}

//--------------------------------------------------------------
bool ofxOpenNI::disableCalibratedRGBDepth() {
	
	// Unregister view point from (image) any map
	if (openNIContext->getDepthGenerator(instanceID).IsCapabilitySupported(XN_CAPABILITY_ALTERNATIVE_VIEW_POINT)) {
		XnStatus nRetVal = openNIContext->getDepthGenerator(instanceID).GetAlternativeViewPointCap().ResetViewPoint();
		ofLogVerbose(LOG_NAME) << "Unregister viewpoint depth to RGB:" << xnGetStatusString(nRetVal);
		if (nRetVal!=XN_STATUS_OK) return false;
	} else {
		ofLogVerbose(LOG_NAME) << "Alternative viewpoint not supported";
		return false;
	}
	
	return true;
}

//--------------------------------------------------------------
void ofxOpenNI::generateIRPixels() {
	// TODO: make this work!!!
	const XnUInt8* pImage = (XnUInt8*)g_IrMD.Data();
//	unsigned char * pixels = backImagePixels->getPixels();
//	int j = 0;
//	for (int i = 0; i < g_IrMD.XRes() * g_IrMD.YRes()*2; i+=2, j++) {		// Don't ask me why ;-)
//		pixels[j] = pImage[i];
//	}
	backImagePixels->setFromPixels(pImage, g_IrMD.XRes(), g_IrMD.YRes(), OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofxOpenNI::generateImagePixels() {
	const XnUInt8* pImage = g_ImageMD.Data();
	backImagePixels->setFromPixels(pImage, g_ImageMD.XRes(), g_ImageMD.YRes(), OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofxOpenNI::generateDepthPixels() {
	
	// get the pixels
	const XnDepthPixel* depth = g_DepthMD.Data();
	
	if (g_DepthMD.FrameID() == 0) return;
	
	// copy raw values
	if (g_bIsDepthRawOnOption) {
		backDepthRawPixels->setFromPixels(depth, g_DepthMD.XRes(), g_DepthMD.YRes(), OF_IMAGE_COLOR);
	}
	
	// copy depth into texture-map
	float max;
	for (XnUInt16 y = g_DepthMD.YOffset(); y < g_DepthMD.YRes() + g_DepthMD.YOffset(); y++) {
		unsigned char * texture = backDepthPixels->getPixels() + y * g_DepthMD.XRes() * 4 + g_DepthMD.XOffset() * 4;
		for (XnUInt16 x = 0; x < g_DepthMD.XRes(); x++, depth++, texture += 4) {
			XnUInt8 red = 0;
			XnUInt8 green = 0;
			XnUInt8 blue = 0;
			XnUInt8 alpha = 255;
			
			XnUInt16 col_index;
			
			switch (depthColoring) {
				case COLORING_PSYCHEDELIC_SHADES:
					alpha *= (((XnFloat)(*depth % 10) / 20) + 0.5);
				case COLORING_PSYCHEDELIC:
					switch ((*depth/10) % 10) {
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
					col_index = (XnUInt16)(((*depth) / (maxDepth / 256)));
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
					col_index = (XnUInt16)(((*depth) / (maxDepth / max)));
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
					XnUInt8 a = (XnUInt8)(((*depth) / (maxDepth / max)));
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
					XnUInt8 a = 255 - (XnUInt8)(((*depth) / (maxDepth / max)));
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
	
	
}

//--------------------------------------------------------------
//xn::Device& ofxOpenNI::getDevice() {
//	return g_Device;
//}

//--------------------------------------------------------------
xn::DepthGenerator& ofxOpenNI::getDepthGenerator() {
	return openNIContext->getDepthGenerator(instanceID);
}

//--------------------------------------------------------------
xn::ImageGenerator& ofxOpenNI::getImageGenerator() {
	return openNIContext->getImageGenerator(instanceID);;
}

//--------------------------------------------------------------
xn::IRGenerator& ofxOpenNI::getIRGenerator() {
	return openNIContext->getIRGenerator(instanceID);;
}

//--------------------------------------------------------------
xn::AudioGenerator& ofxOpenNI::getAudioGenerator() {
	return openNIContext->getAudioGenerator(instanceID);;
}

//--------------------------------------------------------------
xn::Player& ofxOpenNI::getPlayer() {
	return openNIContext->getPlayer(instanceID);;
}

//--------------------------------------------------------------
xn::DepthMetaData& ofxOpenNI::getDepthMetaData() {
	return g_DepthMD;
}

//--------------------------------------------------------------
xn::ImageMetaData& ofxOpenNI::getImageMetaData() {
	return g_ImageMD;
}

//--------------------------------------------------------------
xn::IRMetaData& ofxOpenNI::getIRMetaData() {
	return g_IrMD;
}

//--------------------------------------------------------------
xn::AudioMetaData& ofxOpenNI::getAudioMetaData() {
	return g_AudioMD;
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getDepthPixels() {
	Poco::ScopedLock<ofMutex> lock(mutex);
	return *currentDepthPixels;
}

//--------------------------------------------------------------
ofShortPixels& ofxOpenNI::getDepthRawPixels() {
	Poco::ScopedLock<ofMutex> lock(mutex);
	if (!g_bIsDepthRawOnOption) {
		ofLogWarning(LOG_NAME) << "g_bIsDepthRawOnOption was disabled, enabling raw pixels";
		g_bIsDepthRawOnOption = true;
	}
	return *currentDepthRawPixels;
}

//--------------------------------------------------------------
ofPixels& ofxOpenNI::getImagePixels() {
	Poco::ScopedLock<ofMutex> lock(mutex);
	return *currentImagePixels;
}

//--------------------------------------------------------------
ofTexture& ofxOpenNI::getDepthTextureReference() {
	return depthTexture;
}

//--------------------------------------------------------------
ofTexture& ofxOpenNI::getimageTextureReference() {
	return imageTexture;
}

//--------------------------------------------------------------
float ofxOpenNI::getWidth() {
	if (g_bIsDepthOn) {
		return g_DepthMD.XRes();
	}else if (g_bIsImageOn) {
		return g_ImageMD.XRes();
	}else if (g_bIsIROn) {
		return g_IrMD.XRes();
	} else {
		ofLogWarning(LOG_NAME) << "getWidth() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

//--------------------------------------------------------------
float ofxOpenNI::getHeight() {
	if (g_bIsDepthOn) {
		return g_DepthMD.YRes();
	}else if (g_bIsImageOn) {
		return g_ImageMD.YRes();
	}else if (g_bIsIROn) {
		return g_IrMD.YRes();
	} else {
		ofLogWarning(LOG_NAME) << "getHeight() : We haven't yet initialised any generators, so this value returned is returned as 0";
		return 0;
	}
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::worldToProjective(const ofPoint& p) {
	XnVector3D world = toXn(p);
	return worldToProjective(world);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::worldToProjective(const XnVector3D& p) {
	XnVector3D proj;
	openNIContext->getDepthGenerator(instanceID).ConvertRealWorldToProjective(1,&p,&proj);
	return toOf(proj);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::projectiveToWorld(const ofPoint& p) {
	XnVector3D proj = toXn(p);
	return projectiveToWorld(proj);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::projectiveToWorld(const XnVector3D& p) {
	XnVector3D world;
	openNIContext->getDepthGenerator(instanceID).ConvertProjectiveToRealWorld(1,&p,&world);
	return toOf(world);
}

//--------------------------------------------------------------
ofPoint ofxOpenNI::cameraToWorld(const ofVec2f& c) {
	vector<ofVec2f> vc(1, c);
	vector<ofVec3f> vw(1);
	
	cameraToWorld(vc, vw);
	
	return vw[0];
}

//--------------------------------------------------------------
void ofxOpenNI::cameraToWorld(const vector<ofVec2f>& c, vector<ofVec3f>& w) {
	const int nPoints = c.size();
	w.resize(nPoints);
	if (!g_bIsDepthRawOnOption) {
		ofLogError(LOG_NAME) << "ofxOpenNI::cameraToWorld - cannot perform this function if g_bIsDepthRawOnOption is false. You can enabled g_bIsDepthRawOnOption by calling getDepthRawPixels(..).";
		return;
	}
	
	vector<XnPoint3D> projective(nPoints);
	XnPoint3D *out =&projective[0];
	
	//lock();
	const XnDepthPixel* d = currentDepthRawPixels->getPixels();
	unsigned int pixel;
	for (int i=0; i<nPoints; ++i) {
		pixel  = (int)c[i].x + (int)c[i].y * g_DepthMD.XRes();
		if (pixel >= g_DepthMD.XRes() * g_DepthMD.YRes())
			continue;
		
		projective[i].X = c[i].x;
		projective[i].Y = c[i].y;
		projective[i].Z = float(d[pixel]) / 1000.0f;
	}
	//unlock();
	
	openNIContext->getDepthGenerator(instanceID).ConvertProjectiveToRealWorld(nPoints,&projective[0], (XnPoint3D*)&w[0]);
	
}

//--------------------------------------------------------------
int ofxOpenNI::getNumDevices() {
	return numDevices;
}
