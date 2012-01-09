/*
 * ofxOpenNIRecorder.cpp
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

#include "ofxOpenNIRecorder.h"

// Frame sync is currently not possible with Kinect cameras!!
// undef this if you're using a PrimeSense
#define USINGKINECT

// Rcorder memory consumption (when using cyclic recorder mode -- from NiBackRecorder readme) 
// the recorder stores frames in memory in a cyclic buffer that 
// is dependent on the streams, their resolutions and the amount of time requested.

// Memory consumption
// ------------------
// 1 second, depth VGA: 30*2*640*480 = 18432000 bytes = 17.6MB
// 1 second, depth QVGA: 30*2*320*240 = 4608000 bytes = 4.4MB
// 1 second, image VGA: 30*3*640*480 = 27648000 bytes = 26.4MB
// 1 second, image QVGA: 30*3*320*240 = 6912000 bytes = 6.6MB

// This means:
// 2 minutes, depth QVGA, image QVGA: (4.4MB+6.6MB)*120 = 1.3GB
// 2 minutes, depth VGA, image VGA: (17.6MB+26.4MB)*120 = 5.2GB

// So be warned, the memory consumption can get quite high when 
// recording high resolutions and/or long periods in cyclic mode.

// On the other hand streaming straight to disk doesn't chew memory (just your disk;-)


// Init
//----------------------------------------
ofxOpenNIRecorder::ofxOpenNIRecorder() {
	is_recording = false;
}

// Shutdown
//----------------------------------------
ofxOpenNIRecorder::~ofxOpenNIRecorder() {
	XN_DELETE_ARR(frames);
	stopRecord(); // just to be sure.
}

// Setup the recorder.
//----------------------------------------
void ofxOpenNIRecorder::setup(ofxOpenNIContext*	pContext
							  ,int				b_record_type
							  ,int				b_record_time
							  ,bool				b_record_image
							  ,bool				b_record_ir
							  ,bool				b_record_depth)
{
	
	// set context and generator references
	context = pContext;
	context->getDepthGenerator(&depth_generator);
	if (b_record_image) context->getImageGenerator(&image_generator);
	
	// check we have correct settings for recording image OR ir generators
	if (b_record_ir || (b_record_image && !image_generator.IsValid())) context->getIRGenerator(&ir_generator);
	
	if ((b_record_ir || b_record_image) && !image_generator.IsValid() && ir_generator.IsValid()) {
		printf("Switching recording to IR generator");
		b_record_ir		= true;
		b_record_image	= false;
	}
	if ((b_record_ir || b_record_image) && !image_generator.IsValid() && !ir_generator.IsValid()) {
		printf("No Image or IR generator detected!");
		b_record_ir		= false;
		b_record_image	= false;
	}
	
	// set configuration
	config.record_image = b_record_image;
	config.record_ir	= b_record_ir;
	config.record_depth = b_record_depth;
	config.record_type	= b_record_type;
	config.record_time	= b_record_time;
	
	// set buffer size
	m_nBufferSize = config.record_time * 30;
	frames = XN_NEW_ARR(SingleFrame, m_nBufferSize);
	
	is_recording = false;
	
}

void ofxOpenNIRecorder::setRecordDepth(bool b_record_depth) {
	config.record_depth = b_record_depth;
}

void ofxOpenNIRecorder::setRecordImage(bool b_record_image) {
	config.record_image = b_record_image;
}

void ofxOpenNIRecorder::setRecordType(int b_record_type) {
	config.record_image = b_record_type;
}

void ofxOpenNIRecorder::setRecordTime(int b_record_time) {
	config.record_time = b_record_time;
}

string ofxOpenNIRecorder::getCurrentFileName() {
	return config.record_name;
}

// Start recording (once per recorder)
//----------------------------------------
bool ofxOpenNIRecorder::startRecord(string sName) {
	
	// make sure we don't re-instantiate if we're already recording in stream mode
	if(is_recording && config.record_type == ONI_STREAMING) {
		return false;
	}
	
	xn::MockDepthGenerator	m_depth;
	xn::MockImageGenerator	m_image;
	xn::MockIRGenerator		m_ir;
	
	// reset dropped frame counting variables
	nLastDepthTime = 0;
	nLastImageTime = 0;
	nMissedDepthFrames = 0;
	nMissedImageFrames = 0;
	nDepthFrames = 0;
	nImageFrames = 0;
	
	XnStatus result;
	
	// set the record file name
	config.record_name = ofToDataPath(sName, false);
	
	bool do_init = false;
	
	// by using this do_init method the interface 
	// is transparent to users whichever way we are recording
	// the second call to startRecording when ONI_CYCLING 
	// dumps the buffer to file (see below stopRecord()
	
	if (config.record_type == ONI_CYCLIC && !is_recording) {
		printf("Start cyclic recording: %s\n", config.record_name.c_str());
		
		// reset cyclic recording variables
		m_nNextWrite = 0;
		m_nBufferCount = 0;
		
		is_recording = true;
		
	} else do_init = true; 
	
	if (do_init) {
		
		// recorder init
		result = recorder.Create(context->getXnContext());
		CHECK_RC(result, "Recorder create");
		
		result = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, config.record_name.c_str());
		CHECK_RC(result, "Recorder set destination");
		
		if (config.record_depth) {
			
			if (config.record_type == ONI_STREAMING) {
				
				// just use the depth generator as the node to record
				result = recorder.AddNodeToRecording(depth_generator, XN_CODEC_16Z); // XN_CODEC_16Z_EMB_TABLES is smaller, but seems XN_CODEC_16Z is smoother
				CHECK_RC(result, "Recorder add depth node");
				
			} else if (config.record_type == ONI_CYCLIC) {
				
				// create a mock node based on the depth generator to record
				result = context->getXnContext().CreateMockNodeBasedOn(depth_generator, NULL, m_depth);
				CHECK_RC(result, "Create depth node");
				
				result = recorder.AddNodeToRecording(m_depth, XN_CODEC_16Z); // XN_CODEC_16Z_EMB_TABLES is smaller, but seems XN_CODEC_16Z is smoother
				
				CHECK_RC(result, "Recorder add depth node");
			}
			
		}
		
		// create image node
		if (config.record_image) {
			
			if (config.record_type == ONI_STREAMING) {
				
				// just use the image generator as the node to record
				result = recorder.AddNodeToRecording(image_generator, XN_CODEC_NULL); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
				CHECK_RC(result, "Recorder add image node");
				
			} else if (config.record_type == ONI_CYCLIC) {
				
				// create a mock node based on the image generator to record
				result = context->getXnContext().CreateMockNodeBasedOn(image_generator, NULL, m_image);
				CHECK_RC(result, "Create image node");
				
				result = recorder.AddNodeToRecording(m_image, XN_CODEC_NULL); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
				CHECK_RC(result, "Recorder add image node");
			}
			
		}
		
		// create ir node
		if (config.record_ir) {
			
			if (config.record_type == ONI_STREAMING) {
				
				// just use the image generator as the node to record
				result = recorder.AddNodeToRecording(ir_generator, XN_CODEC_NULL); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
				CHECK_RC(result, "Recorder add ir node");
				
			} else if (config.record_type == ONI_CYCLIC) {
				
				// create a mock node based on the image generator to record
				result = context->getXnContext().CreateMockNodeBasedOn(ir_generator, NULL, m_ir);
				CHECK_RC(result, "Create ir node");
				
				result = recorder.AddNodeToRecording(m_ir, XN_CODEC_NULL); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
				CHECK_RC(result, "Recorder add ir node");
			}
			
		}
		
		// Frame sync is currently not possible with Kinect cameras!!
		// if we try to frame sync then recording fails
#ifndef USINGKINECT	
		// Frame Sync
		if(xn_depth.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC)) {
			if(depth_generator.GetFrameSyncCap().CanFrameSyncWith(image_generator)) {
				result = depth_generator.GetFrameSyncCap().FrameSyncWith(image_generator);
				CHECK_RC(result, "Enable frame sync");
			}
		}
#endif
		
		if (config.record_type == ONI_STREAMING) {
			
			printf("Start streaming recording: %s\n", config.record_name.c_str());
			is_recording = true;
			
		} else if (config.record_type == ONI_CYCLIC && is_recording) {
			
			// Record frames from current position in cyclic buffer loop through to the end
			if (m_nNextWrite < m_nBufferCount) {
				
				// Not first loop, right till end
				for (XnUInt32 i = m_nNextWrite; i < m_nBufferSize; ++i) {
					
					if (config.record_depth)	m_depth.SetData(frames[i].depth_frame);
					if (config.record_image)	m_image.SetData(frames[i].image_frame);
					if (config.record_ir)		m_ir.SetData(frames[i].ir_frame);
					
					recorder.Record();
				}
			}
			
			// Write frames from the beginning of the buffer to the last one written
			for (XnUInt32 i = 0; i < m_nNextWrite; ++i) {
				
				if (config.record_depth)	m_depth.SetData(frames[i].depth_frame);
				if (config.record_image)	m_image.SetData(frames[i].image_frame);
				if (config.record_ir)		m_ir.SetData(frames[i].ir_frame);
				
				recorder.Record();
			}	
			
			// cleanup
			recorder.Release();
			m_ir.Release();
			m_image.Release();
			m_depth.Release();
			
			XN_DELETE_ARR(frames);
		} 
	}
	return true;
}

void ofxOpenNIRecorder::update() {
	
	if (config.record_type == ONI_STREAMING) {
		
		//if (depth_generator.IsDataNew()) {
		//	recorder.Record(); // is this really doing anything??? Don't think so ;-)
		//}
		
	} else if (config.record_type == ONI_CYCLIC && is_recording) {
		
		// store depth frame metadata to cyclic buffer array
		if(config.record_depth) {
			xn::DepthMetaData dmd;
			depth_generator.GetMetaData(dmd);
			frames[m_nNextWrite].depth_frame.CopyFrom(dmd);
		}
		
		// store image frame metadata to cyclic buffer array
		if(config.record_image) {
			xn::ImageMetaData imd;
			image_generator.GetMetaData(imd);
			frames[m_nNextWrite].image_frame.CopyFrom(imd);
		}
		
		// store ir frame metadata to cyclic buffer array
		if(config.record_ir) {
			xn::IRMetaData ird;
			ir_generator.GetMetaData(ird);
			frames[m_nNextWrite].ir_frame.CopyFrom(ird);
		}
		
		// See if buffer is already full
		if (m_nBufferCount < m_nBufferSize) m_nBufferCount++;
		
		// Make sure cylic buffer pointers are good
		m_nNextWrite++;
		
		// Reset cyclic buffer pointer if we're at the end
		if (m_nNextWrite == m_nBufferSize) m_nNextWrite = 0;
	}
	
	
	// Check for missed frames
	if (config.record_depth) {
		
		++nDepthFrames;
		
		XnUInt64 nTimestamp = depth_generator.GetTimestamp();
		
		if (nLastDepthTime != 0 && nTimestamp - nLastDepthTime > 35000) {
			int missed = (int)(nTimestamp-nLastDepthTime)/32000 - 1;
			printf("Missed depth: %llu -> %llu = %d > 35000 - %d frames\n",
				   nLastDepthTime, nTimestamp, XnUInt32(nTimestamp-nLastDepthTime), missed);
			nMissedDepthFrames += missed;
		}
		
		nLastDepthTime = nTimestamp;
		
	}
	
	if (config.record_image) {
		++nImageFrames;
		XnUInt64 nTimestamp = image_generator.GetTimestamp();
		if (nLastImageTime != 0 && nTimestamp - nLastImageTime > 35000) {
			int missed = (int)(nTimestamp-nLastImageTime)/32000 - 1;
			printf("Missed image: %llu -> %llu = %d > 35000 - %d frames\n",
				   nLastImageTime, nTimestamp, XnUInt32(nTimestamp-nLastImageTime), missed);
			nMissedImageFrames += missed;
		}
		nLastImageTime = nTimestamp;
	}
	
	if (config.record_ir) {
		++nImageFrames;
		XnUInt64 nTimestamp = ir_generator.GetTimestamp();
		if (nLastImageTime != 0 && nTimestamp - nLastImageTime > 35000) {
			int missed = (int)(nTimestamp-nLastImageTime)/32000 - 1;
			printf("Missed image (IR): %llu -> %llu = %d > 35000 - %d frames\n",
				   nLastImageTime, nTimestamp, XnUInt32(nTimestamp-nLastImageTime), missed);
			nMissedImageFrames += missed;
		}
		nLastImageTime = nTimestamp;
	}
	
}

// Return recording status
//----------------------------------------
bool ofxOpenNIRecorder::isRecording() {
	return is_recording;
}

// Stop the record
//----------------------------------------
bool ofxOpenNIRecorder::stopRecord() {
	
	if(is_recording){
		
		if (config.record_type == ONI_STREAMING) {
			
			recorder.Release();
			printf("Stopped streaming recording: %s\n", config.record_name.c_str());
			
		} else if (config.record_type == ONI_CYCLIC) {
			
			startRecord(config.record_name); // second call to startRecording dumps the buffer to file
			printf("Stopped cyclic recording: %s\n", config.record_name.c_str());
			
		}
		
		// summarise dropped frames
		if (config.record_depth) printf("Missed %d of %d depth frames (%5.2f%%)\n", 
										nMissedDepthFrames, 
										(nMissedDepthFrames+nDepthFrames), 
										(nMissedDepthFrames*100.0)/(nMissedDepthFrames + nDepthFrames));
		if (config.record_image) printf("Missed %d of %d image frames (%5.2f%%)\n", 
										nMissedImageFrames, 
										(nMissedImageFrames+nImageFrames), 
										(nMissedImageFrames*100.0)/(nMissedImageFrames+nImageFrames));
		
		is_recording = false;
		
	}
	return true;
}
