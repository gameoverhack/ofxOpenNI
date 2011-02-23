#pragma once

#include "ofMain.h"
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>

class ofxDepthGenerator;

class ofxOpenNIContext {
public:
	
	ofxOpenNIContext();
	~ofxOpenNIContext();	
	
	bool setupUsingRecording(std::string sRecordedFile); 
	bool setupUsingXMLFile(std::string sFile = "");
	
	void update();
	
	bool getDepthGenerator(ofxDepthGenerator* pDepthGenerator);
	
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
