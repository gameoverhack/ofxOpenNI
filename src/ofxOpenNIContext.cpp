#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxOpenNIMacros.h"
#include <XnLog.h>

// Startup
//----------------------------------------
ofxOpenNIContext::ofxOpenNIContext()
:is_using_recording(false)
{

}

// Just initialize; use this when you're creating nodes yourself.
//----------------------------------------
bool ofxOpenNIContext::setup(){
	XnStatus result = context.Init();
	BOOL_RC(result, "ofxOpenNIContext.setup()");
}


// Initialize using an .ONI recording.
//----------------------------------------
bool ofxOpenNIContext::setupUsingRecording(std::string sFileRecording) {
	setup();
	addLicense("PrimeSense","0KOIk2JeIBYClPWVnMoRKn5cdY4=");

	is_using_recording = true;
	XnStatus result = XN_STATUS_OK;
	std::string file_path = ofToDataPath(sFileRecording.c_str(), true);
	result = context.OpenFileRecording(file_path.c_str());
	BOOL_RC(result, "Error loading file");
}


// This is used by other nodes (ofxDepthGenerator), which need to 
// use a different initialization when you're loading an recording.
//----------------------------------------
bool ofxOpenNIContext::isUsingRecording() {
	return is_using_recording;
}


// Initialize using an XML file.
//----------------------------------------
bool ofxOpenNIContext::setupUsingXMLFile(std::string sFile) {
	if(sFile == "") {
		sFile = ofToDataPath("openni/config/ofxopenni_config.xml");
		std::cout << sFile << std::endl;
	}
	XnStatus result = XN_STATUS_OK;
	xn::EnumerationErrors errors;
	result = context.InitFromXmlFile(sFile.c_str(),&errors);
	SHOW_RC(result, "init from xml");
	BOOL_RC(result, "ofxOpenNIContext.setupUsingXMLFile()");
}


// When we've been initialized, use this to retrieve the depth
// generator. I.e. when you load from an ONI file the depth generator
// is created automatically and this method is used instead of creating
// it.
//----------------------------------------
bool ofxOpenNIContext::getDepthGenerator(ofxDepthGenerator* pDepthGenerator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(
				XN_NODE_TYPE_DEPTH
				,pDepthGenerator->getXnDepthGenerator()
	);
	BOOL_RC(result, "Error retrieving depth generator");
}

void ofxOpenNIContext::addLicense(std::string sVendor, std::string sKey) {
	XnLicense license = {0};
//	license.strKey =  sKey.c_str();
//	license.strVendor = sVendor.c_str();
	XnStatus status = XN_STATUS_OK;
	status = xnOSStrNCopy(license.strVendor, sVendor.c_str(),sVendor.size(), sizeof(license.strVendor));
	if(status != XN_STATUS_OK) {
		printf("ofxOpenNIContext error creating license (vendor)\n");
		return;
	}
	
	status = xnOSStrNCopy(license.strKey, sKey.c_str(), sKey.size(), sizeof(license.strKey));
	if(status != XN_STATUS_OK) {
		printf("ofxOpenNIContext error creating license (key)\n");
		return;
	}	
	
	status = context.AddLicense(license);
	SHOW_RC(status, "AddLicense");
	
	
	
	xnPrintRegisteredLicenses();
	/*\
	 XnLicense license = {0};
	 if (!bList)
	 {
	 nRetVal = xnOSStrNCopy(license.strVendor, strVendor, strlen(strVendor) + 1, sizeof(license.strVendor));
	 if (nRetVal != XN_STATUS_OK)
	 {
	 printf("Error: Vendor name is too long (should be up to %d characters).", sizeof(license.strVendor) - 1);
	 return -1;
	 }
	 
	 nRetVal = xnOSStrNCopy(license.strKey, strKey, strlen(strKey) + 1, sizeof(license.strKey));
	 if (nRetVal != XN_STATUS_OK)
	 {
	 printf("Error: Key is too long (should be up to %d characters).", sizeof(license.strKey) - 1);
	 return -1;
	 }
	 }
	 
	 |*/
}

void ofxOpenNIContext::enableLogging() {

	XnStatus result = xnLogSetConsoleOutput(true);
	SHOW_RC(result, "Set console output");
			
	result = xnLogSetSeverityFilter(XN_LOG_VERBOSE);
	SHOW_RC(result, "Set log level");
	
	//xnLogInitSystem();
	//xnLogSetConsoleOutput(bVerbose || bList);
	//xnLogSetSeverityFilter(bVerbose ? XN_LOG_VERBOSE : XN_LOG_WARNING);
	xnLogSetMaskState(XN_LOG_MASK_ALL, TRUE);

}


// Update all nodes, should be call in the ofTestApp::update()
//----------------------------------------
void ofxOpenNIContext::update(){
	XnStatus nRetVal = context.WaitAnyUpdateAll();	
	
}


// Get a reference to the xn::Context.
//----------------------------------------
xn::Context& ofxOpenNIContext::getXnContext(){
	return context;
}


// Shutdown.
//----------------------------------------
ofxOpenNIContext::~ofxOpenNIContext(){
	context.Shutdown();
}