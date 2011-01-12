#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxOpenNIMacros.h"

ofxOpenNIContext::ofxOpenNIContext()
:is_using_recording(false)
{

}

bool ofxOpenNIContext::setup(){
	XnStatus nRetVal = context.Init();
	
	if (nRetVal != XN_STATUS_OK){
		printf("Setup OpenNI context failed: %s\n", xnGetStatusString(nRetVal));
		return false;
	} else {
		ofLog(OF_LOG_VERBOSE, "OpenNI Context inited");
		return true;
	}
}

bool ofxOpenNIContext::setupUsingRecording(std::string sFileRecording) {
	is_using_recording = true;
	XnStatus result = XN_STATUS_OK;
	std::string file_path = ofToDataPath(sFileRecording.c_str(), true);
	result = context.OpenFileRecording(file_path.c_str());
	SHOW_RC(result, "Error loading file");
}

bool ofxOpenNIContext::isUsingRecording() {
	return is_using_recording;
}

bool ofxOpenNIContext::setupUsingXMLFile(std::string sFile) {
	if(sFile == "") {
		sFile = ofToDataPath("openni/config/ofxopenni_config.xml");
		std::cout << sFile << std::endl;
	}
	
	XnStatus result = XN_STATUS_OK;
	xn::EnumerationErrors errors;
	result = context.InitFromXmlFile(sFile.c_str(),&errors);

	if (result == XN_STATUS_NO_NODE_PRESENT) {
		XnChar strError[1024];
		errors.ToString(strError, 1024);
		printf("%s\n", strError);
	}
	else if(result != XN_STATUS_OK) {
		printf("+++++ Open failed: %s\n", xnGetStatusString(result));

	}
}

bool ofxOpenNIContext::getDepthGenerator(ofxDepthGenerator* pDepthGenerator) {
	XnStatus result = XN_STATUS_OK;
	result = context.FindExistingNode(
				XN_NODE_TYPE_DEPTH
				,pDepthGenerator->getXnDepthGenerator()
	);
	BOOL_RC(result, "Error retrieving depth generator");
}

void ofxOpenNIContext::update(){
	XnStatus nRetVal = context.WaitAnyUpdateAll();	
	
}

xn::Context& ofxOpenNIContext::getXnContext(){
	return context;
}
ofxOpenNIContext::~ofxOpenNIContext(){
	context.Shutdown();
}