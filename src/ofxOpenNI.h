#ifndef	_H_OFXOPENNI
#define _H_OFXOPENNI

#include "ofxOpenNIContext.h"
#include "ofxImageGenerator.h"
#include "ofxIRGenerator.h"
#include "ofxDepthGenerator.h"
#include "ofxUserGenerator.h"
#include "ofxTrackedUser.h"
#include "ofxImageGenerator.h"
#include "ofxHandGenerator.h"
#include "ofxTrackedHand.h"
#include "ofxOpenNIRecorder.h"

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
#include "ofxHardwareDriver.h"
#endif

#endif
