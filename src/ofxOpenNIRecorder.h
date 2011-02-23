#pragma once

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"
#include "ofxUserGenerator.h"
#include "ofxOpenNIMacros.h"
#include <XnPropNames.h>

// Record Type
enum RecordType {
	ONI_CYCLIC,
	ONI_STREAMING,
};

const string RecordType[] = {
	"ONI_CYCLIC",
	"ONI_STREAMING",
};

// Record Configuration
struct RecordConfiguration {
	RecordConfiguration() {
		
		record_depth = true;
		record_image = true;
		record_type	 = ONI_STREAMING;
		record_name	 = "";
		
	}
	bool		record_depth;
	bool		record_image;
	int			record_type;
	string		record_name;
};


class ofxOpenNIRecorder {
public:
	ofxOpenNIRecorder();
	~ofxOpenNIRecorder();
	
	void setup(	ofxOpenNIContext*	pContext
			   ,ofxDepthGenerator*	pDepth
			   ,ofxImageGenerator*	pImage
			   ,int					b_record_type	= ONI_STREAMING
			   ,bool				b_record_image	= true
			   ,bool				b_record_depth	= true);
	
	void update();
	
	void setRecordDepth(bool bShouldRecordDepth);
	void setRecordImage(bool bShouldRecordImage);
	void setRecordType(int b_record_type);
	
	string getCurrentFileName();
	
	bool startRecord(string fname);
	
	bool stopRecord();

	bool isRecording();
	
private:
	
	ofxOpenNIContext*	context;	
	ofxDepthGenerator*	depth_generator;
	ofxImageGenerator*	image_generator;

	XnStatus setDepthFrame(xn::DepthGenerator& rDepth, const xn::DepthMetaData& rDMD);
	XnStatus setImageFrame(xn::ImageGenerator& rImage, const xn::ImageMetaData& rIMD);
	
	struct SingleFrame {
		xn::DepthMetaData depth_frame;
		xn::ImageMetaData image_frame;
	};
	
	std::vector<SingleFrame*> frames;
	
	RecordConfiguration config;
	
	bool is_recording;	
	xn::Recorder recorder;
	
};
