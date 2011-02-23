#include "ofxOpenNIRecorder.h"

// Frame sync is currently not possible with Kinect cameras!!
// undef this if you're using a PrimeSense
#define USINGKINECT

// Init
//----------------------------------------
ofxOpenNIRecorder::ofxOpenNIRecorder() {
	is_recording = false;
}


// Shutdown
//----------------------------------------
ofxOpenNIRecorder::~ofxOpenNIRecorder() {
	stopRecord(); // just to be sure.
}


// Setup the recorder.
//----------------------------------------
void ofxOpenNIRecorder::setup(
							  ofxOpenNIContext*	pContext
							  ,ofxDepthGenerator*	pDepth
							  ,ofxImageGenerator*	pImage
							  ,int				b_record_type
							  ,bool				b_record_image
							  ,bool				b_record_depth)
{
	
	context = pContext;
	depth_generator = pDepth;
	image_generator = pImage;
	
	config.record_image = b_record_image;
	config.record_depth = b_record_depth;
	config.record_type	= b_record_type;
	
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
	
	xn::MockDepthGenerator m_depth;
	xn::MockImageGenerator m_image;
	xn::EnumerationErrors errors;
	
	XnStatus result;
	
	config.record_name = ofToDataPath(sName, false);
	
	bool do_init = false;
	
	if (config.record_type == ONI_CYCLIC && !is_recording) {
		printf("Start cyclic recording: %s\n", config.record_name.c_str());
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
				result = recorder.AddNodeToRecording(depth_generator->getXnDepthGenerator(), XN_CODEC_16Z_EMB_TABLES);
				CHECK_RC(result, "Recorder add depth node");
				
			} else if (config.record_type == ONI_CYCLIC) {
				
				// create a mock node based on the depth generator to record
				result = context->getXnContext().CreateMockNodeBasedOn(depth_generator->getXnDepthGenerator()
																	   ,NULL
																	   ,m_depth);
				CHECK_RC(result, "Create depth node");
				
				result = recorder.AddNodeToRecording(m_depth, XN_CODEC_16Z_EMB_TABLES);
				CHECK_RC(result, "Recorder add depth node");
			}
			
			
			
		}
		
		// create image node
		if (config.record_image) {
			if (config.record_type == ONI_STREAMING) {
				
				result = recorder.AddNodeToRecording(image_generator->getXnImageGenerator(), XN_CODEC_NULL); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
				CHECK_RC(result, "Recorder add image node");
				
			} else if (config.record_type == ONI_CYCLIC) {
				
				// create a mock node based on the image generator to record
				result = context->getXnContext().CreateMockNodeBasedOn(image_generator->getXnImageGenerator()
																	   ,NULL
																	   ,m_image);
				CHECK_RC(result, "Create image node");
				
				result = recorder.AddNodeToRecording(m_image, XN_CODEC_JPEG); // XN_CODEC_NULL appears to give least frame drops and size not much > JPEG
				CHECK_RC(result, "Recorder add image node");
			}
			
		}
		
		// Frame sync is currently not possible with Kinect cameras!!
		// if we try to frame sync then recording fails
#ifndef USINGKINECT	
		// Frame Sync
		xn::DepthGenerator& xn_depth = depth->getXnDepthGenerator();
		xn::ImageGenerator& xn_image = image->getXnImageGenerator();
		
		if(xn_depth.IsCapabilitySupported(XN_CAPABILITY_FRAME_SYNC)) {
			if(xn_depth.GetFrameSyncCap().CanFrameSyncWith(xn_image)) {
				result = xn_depth.GetFrameSyncCap().FrameSyncWith(xn_image);
				CHECK_RC(result, "Enable frame sync");
			}
		}
		
#endif
		
		if (config.record_type == ONI_STREAMING) {
			
			printf("Start streaming recording: %s\n", config.record_name.c_str());
			is_recording = true;
			
		} else if (config.record_type == ONI_CYCLIC && is_recording) {
			
			// store frames.
			std::vector<SingleFrame*>::iterator it_frame = frames.begin();
			while(it_frame != frames.end()) {
				if(config.record_depth) {
					m_depth.SetData((*it_frame)->depth_frame);
					//setDepthFrame(r_depth, (*it_frame)->depth_frame);
					
				}
				if(config.record_image) {
					m_image.SetData((*it_frame)->image_frame);
					//setImageFrame(r_image, (*it_frame)->image_frame);
					
				}
				++it_frame;
				
				recorder.Record();
				
			}	
			
			// cleanup
			recorder.Unref();
			m_image.Unref();
			m_depth.Unref();
			
			it_frame = frames.begin();
			while(it_frame != frames.end()) {
				delete (*it_frame);
				it_frame = frames.erase(it_frame);
			}
		} 
	}
	return true;
}

void ofxOpenNIRecorder::update() {
	
	if (config.record_type == ONI_STREAMING) {
		
		recorder.Record(); // is this really doing anything?
		
	} else if (config.record_type == ONI_CYCLIC && is_recording) {
		SingleFrame* frame = new SingleFrame();
		
		if(config.record_depth) {
			xn::DepthMetaData dmd;
			depth_generator->getXnDepthGenerator().GetMetaData(dmd);
			frame->depth_frame.CopyFrom(dmd);
		}
		
		
		if(config.record_image) {
			xn::ImageMetaData imd;
			image_generator->getXnImageGenerator().GetMetaData(imd);
			frame->image_frame.CopyFrom(imd);
		}
		
		frames.push_back(frame);
		
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
			
			recorder.Unref();
			printf("Stopped streaming recording: %s\n", config.record_name.c_str());
			
		} else if (config.record_type == ONI_CYCLIC) {
			
			startRecord(config.record_name);
			printf("Stopped cyclic recording: %s\n", config.record_name.c_str());
			
		}
		
		is_recording = false;
		
	}
	return true;
}
