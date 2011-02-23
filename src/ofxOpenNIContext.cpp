#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxOpenNIMacros.h"
#include <XnLog.h>

// Startup
//----------------------------------------
ofxOpenNIContext::ofxOpenNIContext() {
	is_using_recording = false;
}

// Just initialize; use this when you're creating nodes yourself.
//----------------------------------------
bool ofxOpenNIContext::initContext(){
	XnStatus result = context.Init();
	BOOL_RC(result, "ofxOpenNIContext.setup()");
}

// Initialize using an .ONI recording.
//----------------------------------------
bool ofxOpenNIContext::setupUsingRecording(std::string sFileRecording) {
	
	initContext();
	addLicense("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
	
	is_using_recording = true;
	
	std::string file_path = ofToDataPath(sFileRecording.c_str(), true);
	
	printf("Attempting to open file: %s\n", file_path.c_str());
	
	XnStatus result = context.OpenFileRecording(file_path.c_str());
	
	BOOL_RC(result, "Loading file");
}

void ofxOpenNIContext::logErrors(xn::EnumerationErrors& rErrors) {
	for(xn::EnumerationErrors::Iterator it = rErrors.Begin(); it != rErrors.End(); ++it) {
		XnChar desc[512];
		xnProductionNodeDescriptionToString(&it.Description(), desc,512);
		printf("%s failed: %s\n", desc, xnGetStatusString(it.Error()));
	}	
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
		sFile = ofToDataPath("openni/config/ofxopenni_config.xml",true);
	}
	
	std::cout << "Using file:" << sFile << std::endl;

	xn::EnumerationErrors errors;
	XnStatus result = context.InitFromXmlFile(sFile.c_str(),&errors);
	
	if(result != XN_STATUS_OK) {
		logErrors(errors);
	}


	BOOL_RC(result, "ofxOpenNIContext.setupUsingXMLFile()");
}


// When we've been initialized, use this to retrieve the depth
// generator. I.e. when you load from an ONI file the depth generator
// is created automatically and this method is used instead of creating it.
//----------------------------------------
bool ofxOpenNIContext::getDepthGenerator(ofxDepthGenerator* pDepthGenerator) {
	
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(
				XN_NODE_TYPE_DEPTH
				,pDepthGenerator->getXnDepthGenerator()
	);
	BOOL_RC(result, "Retrieving depth generator");
	
}

// we need to programmatically add a license when playing back a recording
// file otherwise the skeleton tracker will throw an error and not work
void ofxOpenNIContext::addLicense(std::string sVendor, std::string sKey) {
	XnLicense license = {0};
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

}

// TODO: check this is working and use it with ONI recordings??
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

// Allow us to mirror the image_gen/depth_gen
bool ofxOpenNIContext::toggleMirror() {
	return setMirror(!context.GetGlobalMirror());
}

bool ofxOpenNIContext::setMirror(XnBool mirroring) {
	XnStatus result = context.SetGlobalMirror(mirroring);
	BOOL_RC(result, "Set mirroring");
}

// Get a reference to the xn::Context.
//----------------------------------------
xn::Context& ofxOpenNIContext::getXnContext(){
	return context;
}

void ofxOpenNIContext::shutdown() {
	printf("Shutdown context\n");
	context.Shutdown();
}

// Shutdown.
//----------------------------------------
ofxOpenNIContext::~ofxOpenNIContext(){
	shutdown();
}