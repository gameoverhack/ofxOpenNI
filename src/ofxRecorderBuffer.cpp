#include "ofxRecorderBuffer.h"
ofxRecorderBuffer::ofxRecorderBuffer() 
{
	config.record_image = true;
	config.record_depth = true;
}


void ofxRecorderBuffer::setup(
	std::string sDestFile
	,ofxOpenNIContext* pContext
	,ofxDepthGenerator* pDepthGenerator
	,ofxImageGenerator* pImageGenerator
)
{
	destination_file = ofToDataPath(sDestFile.c_str(), true);
	context = pContext;
	depth_generator = pDepthGenerator;
	image_generator = pImageGenerator;
}



void ofxRecorderBuffer::setRecordDepth(bool bShouldRecordDepth) {
	config.record_depth = bShouldRecordDepth;
}



void ofxRecorderBuffer::setRecordImage(bool bShouldRecordImage) {
	config.record_image = bShouldRecordImage;
}



void ofxRecorderBuffer::update() {
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




XnStatus ofxRecorderBuffer::setDepthFrame(
	xn::DepthGenerator& rDepth
	,const xn::DepthMetaData& rDMD
)
{
	XnStatus result = XN_STATUS_OK;
	result = rDepth.SetGeneralProperty(XN_PROP_NEWDATA, rDMD.DataSize(), rDMD.Data());
	CHECK_RC(result, "Set new depth data");
	
	result = rDepth.SetIntProperty(XN_PROP_FRAME_ID, rDMD.FrameID());
	CHECK_RC(result, "Set frame id");
	
	//result = rDepth.SetIntProperty(XN_PROP_TIMESTAMP, rDMD.Timestamp());
	//CHECK_RC(result, "Set timestamp");
	
	rDepth.WaitAndUpdateData(); // why?
	CHECK_RC(result, "Mock node update data");
	
	std::cout << "setDethFrame" << std::endl;
	return XN_STATUS_OK;
}




XnStatus ofxRecorderBuffer::setImageFrame(
	 xn::ImageGenerator& rImage
	,const xn::ImageMetaData& rIMD
)
{
	XnStatus result = XN_STATUS_OK;
	result = rImage.SetGeneralProperty(XN_PROP_NEWDATA, rIMD.DataSize(), rIMD.Data());
	CHECK_RC(result, "Set mock node new data");
	
	result = rImage.SetIntProperty(XN_PROP_FRAME_ID, rIMD.FrameID());
	CHECK_RC(result, "Set mock node frame id");
	
	//result = rImage.SetIntProperty(XN_PROP_TIMESTAMP, rIMD.Timestamp());
	//CHECK_RC(result, "Set mock node frame id");
	
	result = rImage.WaitAndUpdateData();
	CHECK_RC(result, "Mock node update data");
	
	std::cout << "setImageFrame\n";
	return XN_STATUS_OK;
}



XnStatus ofxRecorderBuffer::dump() {
	xn::DepthGenerator mock_depth;
	xn::ImageGenerator mock_image;
	xn::EnumerationErrors errors;
	XnStatus result;
	
	// create recorder.
	result = context->getXnContext().CreateAnyProductionTree(XN_NODE_TYPE_RECORDER, NULL, recorder, &errors);
	CHECK_RC_ERR(result, "Create recorder", errors);
	
	recorder.SetDestination(XN_RECORD_MEDIUM_FILE, destination_file.c_str());
	
	// create mock nodes
	// ------------------
	if(config.record_depth) {
		result = context->getXnContext().CreateMockNodeBasedOn(
					depth_generator->getXnDepthGenerator()
					,NULL
					,mock_depth
		);
		CHECK_RC(result, "Create depth node");
		
		result = recorder.AddNodeToRecording(mock_depth, XN_CODEC_16Z_EMB_TABLES);
		CHECK_RC(result, "Add depth node");
	}
	
	
	// create image node
	if(config.record_image) {
		result = context->getXnContext().CreateMockNodeBasedOn(
					image_generator->getXnImageGenerator()
					,NULL
					,mock_image
		);
		CHECK_RC(result, "Create image node");
		
		result = recorder.AddNodeToRecording(mock_image, XN_CODEC_JPEG);
		CHECK_RC(result, "Add image node");
	}
	
	// store frames.
	std::vector<SingleFrame*>::iterator it_frame = frames.begin();
	while(it_frame != frames.end()) {
		if(config.record_depth) {
			setDepthFrame(mock_depth, (*it_frame)->depth_frame);
		}
		if(config.record_image) {
			setImageFrame(mock_image, (*it_frame)->image_frame);
		}
		++it_frame;
	}	
	
	recorder.Record();
	
	
	// cleanup
	recorder.Unref();
	mock_image.Unref();
	mock_depth.Unref();

	it_frame = frames.begin();
	while(it_frame != frames.end()) {
		delete (*it_frame);
		it_frame = frames.erase(it_frame);
	}	
	return result;
}