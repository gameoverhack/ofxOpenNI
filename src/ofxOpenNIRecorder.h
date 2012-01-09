/*
 * ofxOpenNIRecorder.h
 *
 * Copyright 2011 (c) Matthew Gingold http://gingold.com.au
 * Originally forked from a project by roxlu http://www.roxlu.com/ 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

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