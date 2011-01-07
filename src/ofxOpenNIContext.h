#ifndef OFXOPENNICONTEXTH
#define OFXOPENNICONTEXTH

#include "ofMain.h"

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>


class ofxOpenNIContext {
public:
	ofxOpenNIContext();
	~ofxOpenNIContext();	

	bool setup();
	void update();
	bool initFromXMLFile(std::string sFile = "");
	xn::Context* getXnContext();

	
private:
	xn::Context context;
//	xn::UserGenerator g_UserGenerator;
	
};

#endif