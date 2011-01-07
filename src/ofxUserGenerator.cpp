#include "ofxUserGenerator.h"
#include "ofxOpenNIMacros.h"

ofxUserGenerator::ofxUserGenerator() {
}

bool ofxUserGenerator::setup(ofxOpenNIContext* pContext) {
	context			= pContext;
	XnStatus result = XN_STATUS_OK;
	
	// check if the USER generator exists.
	result = context
				->getXnContext()
				->FindExistingNode(XN_NODE_TYPE_USER, user_generator);
	CHECK_RC(result, "Find user generator");
	if(result != XN_STATUS_OK) {
		return false;
	}
	
	// create user generator.
	result = user_generator.Create(*context->getXnContext());
	CHECK_RC(result, "Create user generator");
	if(result != XN_STATUS_OK) {
		return false;
	}
	return true;
}
