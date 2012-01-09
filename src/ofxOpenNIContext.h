/*
 * ofxOpenNIContext.h
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

#ifndef _H_OFXOPENNICONTEXT
#define _H_OFXOPENNICONTEXT

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include "ofMain.h"

class ofxOpenNIContext {

public:

	ofxOpenNIContext();
	~ofxOpenNIContext();

	bool setup();
	bool setupUsingRecording(std::string sRecordedFile);
	bool setupUsingXMLFile(std::string sFile = "");

	void update();

	bool toggleRegisterViewport();
	bool registerViewport();
	bool unregisterViewport();

	bool getDepthGenerator(xn::DepthGenerator* depth_generator);
	bool getImageGenerator(xn::ImageGenerator* image_generator);
	bool getIRGenerator(xn::IRGenerator* ir_generator);
	bool getUserGenerator(xn::UserGenerator* user_generator);
	bool getGestureGenerator(xn::GestureGenerator* gesture_generator);
	bool getHandsGenerator(xn::HandsGenerator* hands_generator);
	bool isUsingRecording();

	void enableLogging();

	bool toggleMirror();
	bool setMirror(XnBool mirroring);

	void shutdown();

	xn::Context& getXnContext();

private:

	bool initContext();
	void addLicense(std::string sVendor, std::string sKey);
	void logErrors(xn::EnumerationErrors& rErrors);

	bool is_using_recording;
	xn::Context context;

};

#endif
