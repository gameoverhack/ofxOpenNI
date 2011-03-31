#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxOpenNIMacros.h"

// Startup
//----------------------------------------
ofxOpenNIContext::ofxOpenNIContext()
:is_using_recording(false),
is_initialized(false)
{

}

// Just initialize; use this when you're creating nodes yourself.
//----------------------------------------
bool ofxOpenNIContext::setup(){
	XnStatus result = context.Init();
	BOOL_RC(result, "ofxOpenNIContext.setup()");
}

bool ofxOpenNIContext::toggleMirror() {
	XnStatus result = context.SetGlobalMirror(!context.GetGlobalMirror());
	BOOL_RC(result, "toggleMirror");
}

// Initialize using an .ONI recording.
//----------------------------------------
bool ofxOpenNIContext::setupUsingRecording(std::string sFileRecording) {
	
	setup();
	addLicense("PrimeSense", "0KOIk2JeIBYClPWVnMoRKn5cdY4=");
	
	is_using_recording = true;
	XnStatus result = XN_STATUS_OK;
	std::string file_path = ofToDataPath(sFileRecording.c_str(), true);
	
	printf("Attempting to open file: %s\n", file_path.c_str());
	
	result = context.OpenFileRecording(file_path.c_str());
	
	if(result != XN_STATUS_OK) {
		cout << "FILE ERROR" << endl;
		is_initialized = false;
	} else {
		cout << "FILE OK" << endl;
		is_initialized = true;
	}
	
	BOOL_RC(result, "Error loading file");
}

bool ofxOpenNIContext::setupUsingXMLObject(ofxOpenNIXML oXML) {
	std::string xml = oXML.getXML();
	string tmp_name = ofToDataPath("tmp.xml",true);
	ofstream ofs(tmp_name.c_str());
	ofs << xml.c_str();
	ofs.close();
	setupUsingXMLFile(tmp_name);
	//return runXMLScript(xml);
	return true;
}

bool ofxOpenNIContext::runXMLScript(std::string sXML) {
	XnStatus result = XN_STATUS_OK;
	xn::EnumerationErrors errors;
	result = context.RunXmlScript(sXML.c_str(),&errors);
	if(result != XN_STATUS_OK) {
		logErrors(errors);		
		return false;
	}
	return true;
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


bool ofxOpenNIContext::isInitialized() {
	return is_initialized;
}

// Initialize using an XML file.
//----------------------------------------
bool ofxOpenNIContext::setupUsingXMLFile(std::string sFile) {
	
	if(sFile == "") {
		sFile = ofToDataPath("openni/config/ofxopenni_config.xml",true);
	}
	
	std::cout << "Using file:" << sFile << std::endl;
	
	XnStatus result = XN_STATUS_OK;
	xn::EnumerationErrors errors;
	result = context.InitFromXmlFile(sFile.c_str(),&errors);
	if(result != XN_STATUS_OK) {
		for(xn::EnumerationErrors::Iterator it = errors.Begin(); it != errors.End(); ++it) {
			XnChar desc[512];
			xnProductionNodeDescriptionToString(&it.Description(), desc,512);
			printf("%s failed to to enumerate: %s\n", desc, xnGetStatusString(it.Error()));
		}
		is_initialized = false;
	}
	else {
		is_initialized = true;
	}

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

void ofxOpenNIContext::enableLogging(XnLogSeverity level) {

	XnStatus result = xnLogSetConsoleOutput(true);
	SHOW_RC(result, "Set console output");
			
	result = xnLogSetSeverityFilter(level);
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

void ofxOpenNIContext::clear() {
	printf("Shutdown\n");
	context.Shutdown();
}

// Shutdown.
//----------------------------------------
ofxOpenNIContext::~ofxOpenNIContext(){

	clear();

}