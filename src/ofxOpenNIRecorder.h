#pragma once
#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofxImageGenerator.h"
#include "ofxOpenNIMacros.h"

class ofxOpenNIRecorder {
public:
	ofxOpenNIRecorder(){
		bRecording = false;
	}
	
	void setup(ofxOpenNIContext* context, ofxDepthGenerator * depth, ofxImageGenerator * image){
		_depth = depth;
		_image = image;
		_context = context;
	}
	
	bool startRecord(string fname) {
		
		//stop();
		
		XnStatus nRetVal;
		
		string offname = ofToDataPath(fname, false);
		
		// recorder init
		//---------------------------------------------------------------------
		nRetVal = recorder.Create(_context->getXnContext());
		CHECK_RC(nRetVal, "Recorder create");
		
		nRetVal = recorder.SetDestination(XN_RECORD_MEDIUM_FILE, offname.c_str());
		CHECK_RC(nRetVal, "Recorder set destination");
		
		
		nRetVal = recorder.AddNodeToRecording(_depth->getXnDepthGenerator(), XN_CODEC_16Z_EMB_TABLES);
		CHECK_RC(nRetVal, "Recorder add depth node");
		
		nRetVal = recorder.AddNodeToRecording(_image->getXnImageGenerator(), XN_CODEC_JPEG);
		CHECK_RC(nRetVal, "Recorder add image node");
		
		bRecording = true;
		return true;
	}
	
	bool stopRecord() {
		
		if(bRecording){
			recorder.Unref();
			bRecording = false;
		}
		
		return true;
	}
	bool bRecording;

private:
	ofxOpenNIContext * _context;	
	ofxDepthGenerator * _depth;
	ofxImageGenerator * _image;
	
	xn::Recorder recorder;
	
};
