#include "ofxOpenNIPlayback.h"
#include "ofxDepthGenerator.h"
#include "ofxOpenNIContext.h"
#include "ofxOpenNIMacros.h"
 
 

ofxOpenNIPlayback::ofxOpenNIPlayback() {
}

void ofxOpenNIPlayback::setup(ofxOpenNIContext* pContext) {
	context = pContext;
}

void ofxOpenNIPlayback::play(std::string sFileRecording) {
	/*
	XnStatus result = XN_STATUS_OK;
	std::string file_path = ofToDataPath(sFileRecording.c_str(), true);
	result = context->getXnContext().OpenFileRecording(file_path.c_str());
	SHOW_RC(result, "Error loading file");
							 printf("File:%s\n", file_path.c_str());
	 */
	
}


void ofxOpenNIPlayback::getDepthGenerator(ofxDepthGenerator* pDepthGenerator) {
	/*
	XnStatus result = XN_STATUS_OK;
	result = context->getXnContext().FindExistingNode(XN_NODE_TYPE_DEPTH, *pDepthGenerator->getXnDepthGenerator());
	SHOW_RC(result, "Error retrieving depth generator");
	if(result != XN_STATUS_OK) {
		printf("Error retrieving the depth generator\n");
	}
	if(result) {
		printf("Loaded depth generator\n");
	}
	pDepthGenerator->setup();
	*/
}
