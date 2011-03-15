#pragma once

#include "ofMain.h"
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>


//class ofxIRGenerator;
//class ofxImageGenerator;
//class ofxDepthGenerator;
class ofxOpenNIContext {
public:
	
	ofxOpenNIContext();
	~ofxOpenNIContext();	
	
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
