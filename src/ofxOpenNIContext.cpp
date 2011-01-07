#include "ofxOpenNIContext.h"
ofxOpenNIContext::ofxOpenNIContext(){
}

bool ofxOpenNIContext::setup(){
	//Init OpenNI context
	XnStatus nRetVal = context.Init();
	
	if (nRetVal != XN_STATUS_OK){
		printf("Setup OpenNI context failed: %s\n", xnGetStatusString(nRetVal));
		return false;
	} else {
		ofLog(OF_LOG_VERBOSE, "OpenNI Context inited");
		return true;
	}
}

bool ofxOpenNIContext::initFromXMLFile(std::string sFile) {
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


void ofxOpenNIContext::update(){
	XnStatus nRetVal = context.WaitAnyUpdateAll();	
	
}

xn::Context * ofxOpenNIContext::getXnContext(){
	return &context;
}
ofxOpenNIContext::~ofxOpenNIContext(){
	context.Shutdown();
}