#pragma once

#include "ofMain.h"
#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include "ofxOpenNIXML.h"
#include <fstream>
class ofxDepthGenerator;

class ofxOpenNIContext {
public:
	ofxOpenNIContext();
	
	~ofxOpenNIContext();	


	void update();
	
	bool setup();
	
	bool setupUsingXMLFile(std::string sFile = "");
	
	bool setupUsingRecording(std::string sRecordedFile); 
	
	bool setupUsingXMLObject(ofxOpenNIXML oXML);
	
	bool runXMLScript(std::string sXML);
	
	xn::Context& getXnContext();
	
	bool getDepthGenerator(ofxDepthGenerator* pDepthGenerator);

	bool isUsingRecording();
	
	void addLicense(std::string sVendor, std::string sKey);
	
	void enableLogging();
	
	bool isInitialized();
	
private:
	void logErrors(xn::EnumerationErrors& rErrors);
	
	bool is_initialized;
	bool is_using_recording;
	xn::Context context;
};
