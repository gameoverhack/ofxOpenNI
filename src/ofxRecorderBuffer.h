#pragma once 
#include "ofxOpenNI.h"
#include <XnPropNames.h>
#include <vector>

// Configuration
struct RecConfiguration {
	RecConfiguration() {
	
		record_depth = FALSE;
		record_image = FALSE;
	
	}
	XnBool record_depth;
	XnBool record_image;
};


// Used the cyclicbuffer from the openni examples
class ofxRecorderBuffer {

public:
	ofxRecorderBuffer();
	
	void setup(
		std::string sDestFile
		,ofxOpenNIContext* pContext
		,ofxDepthGenerator* pDepthGenerator
		,ofxImageGenerator* pImageGenerator
	);
	
	void setRecordDepth(bool bShouldRecordDepth);
	void setRecordImage(bool bShouldRecordImage);
	
	void update(); 
	XnStatus dump();
	
private:
	
	XnStatus setDepthFrame(xn::DepthGenerator& rDepth, const xn::DepthMetaData& rDMD);
	XnStatus setImageFrame(xn::ImageGenerator& rImage, const xn::ImageMetaData& rIMD);
	
	struct SingleFrame {
		xn::DepthMetaData depth_frame;
		xn::ImageMetaData image_frame;
	};
	std::vector<SingleFrame*> frames;
	
	ofxOpenNIContext* context;
	ofxDepthGenerator* depth_generator;
	ofxImageGenerator* image_generator;
	std::string destination_file;
	xn::Recorder recorder;
	RecConfiguration config;
};
