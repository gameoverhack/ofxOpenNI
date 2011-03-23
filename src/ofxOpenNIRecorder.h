#ifndef _H_OFXOPENNIRECORDER
#define _H_OFXOPENNIRECORDER

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
		record_ir	 = false;
		record_time  = 0;
		record_type	 = ONI_STREAMING;
		record_name	 = "";
		
	}
	bool		record_depth;
	bool		record_image;
	bool		record_ir;
	int			record_time;
	int			record_type;
	string		record_name;
};


class ofxOpenNIRecorder {
	
public:
	ofxOpenNIRecorder();
	~ofxOpenNIRecorder();
	
	void setup(	ofxOpenNIContext*	pContext
			   ,int					b_record_type	= ONI_STREAMING
			   ,int					b_record_time	= 0
			   ,bool				b_record_image	= true
			   ,bool				b_record_ir		= false
			   ,bool				b_record_depth	= true);
	
	void update();
	
	void setRecordDepth(bool bShouldRecordDepth);
	void setRecordImage(bool bShouldRecordImage);
	void setRecordType(int b_record_type);
	void setRecordTime(int b_record_time);
	
	string getCurrentFileName();
	
	bool startRecord(string fname);
	
	bool stopRecord();

	bool isRecording();
	
private:
	
	xn::Recorder		recorder;
	
	ofxOpenNIContext*	context;	
	xn::DepthGenerator	depth_generator;
	xn::ImageGenerator	image_generator;
	xn::IRGenerator		ir_generator;
	
	struct SingleFrame {
		xn::DepthMetaData	depth_frame;
		xn::ImageMetaData	image_frame;
		xn::IRMetaData		ir_frame;
	};
	
	SingleFrame*		frames;
	XnUInt32 m_nNextWrite;
	XnUInt32 m_nBufferSize;
	XnUInt32 m_nBufferCount;
	
	// To count missed frames
	XnUInt64 nLastDepthTime;
	XnUInt64 nLastImageTime;
	XnUInt32 nMissedDepthFrames;
	XnUInt32 nMissedImageFrames;
	XnUInt32 nDepthFrames;
	XnUInt32 nImageFrames;
	
	RecordConfiguration config;
	
	bool				is_recording;	

	
};

#endif