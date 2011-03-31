#include "ofxGestureGenerator.h"
#include "ofxOpenNIMacros.h"


// GESTURE CALLBACKS
// =============================================================================
// Callback: New Gesture was detected
void XN_CALLBACK_TYPE
Gesture_Recognized(
	xn::GestureGenerator& generator,
	const XnChar* strGesture,
	const XnPoint3D* pIDPosition,
	const XnPoint3D* pEndPosition, void* pCookie
)
{
	printf("Gesture recognized: %s  id [%d/%d]  pos [%d]%d]\n", strGesture,(int)pIDPosition->X,(int)pIDPosition->Y,(int)pEndPosition->X,(int)pEndPosition->Y);
	ofxGestureGenerator* gest = static_cast<ofxGestureGenerator*>(pCookie);
	gest->startHandTracking(pEndPosition);
}

// Callback: Gesture processing
void XN_CALLBACK_TYPE
Gesture_Process(
	xn::GestureGenerator& generator,
	const XnChar* strGesture,
	const XnPoint3D* pPosition,
	XnFloat fProgress,
	void* pCookie)
{
}


// HANDS CALLBACKS
// =============================================================================
// Callback: New Hand was detected
void XN_CALLBACK_TYPE HandCreate(
	xn::HandsGenerator& generator, 
	XnUserID nID, 
	const XnPoint3D* pPosition, 
	XnFloat fTime, 
	void* pCookie
) 
{
	printf("New Hand: %d\n", nID);
	ofxGestureGenerator* gest = static_cast<ofxGestureGenerator*>(pCookie);
	gest->newHand(nID, pPosition);
}

// Callback: A Hand has moved
void XN_CALLBACK_TYPE HandUpdate(
	 xn::HandsGenerator& generator, 
	 XnUserID nID, 
	 const XnPoint3D* pPosition, 
	 XnFloat fTime, void* 
	 pCookie
)
{
	//printf("Hand update %d\n", user);
	ofxGestureGenerator* gest = static_cast<ofxGestureGenerator*>(pCookie);
	gest->updateHand(nID, pPosition);
}

// Callback: A Hand was lost :(
void XN_CALLBACK_TYPE HandDestroy(
	xn::HandsGenerator& generator, 
	XnUserID nID, 
	XnFloat fTime, 
	void* pCookie
)
{
	printf("Hand Lost: %d\n", nID);
	ofxGestureGenerator* gest = static_cast<ofxGestureGenerator*>(pCookie);
	gest->destroyHand(nID);
}


//--------------------------------------------------------------
ofxGestureGenerator::ofxGestureGenerator(){
}

//--------------------------------------------------------------
bool ofxGestureGenerator::setup(ofxOpenNIContext* pContext, ofxDepthGenerator* pDepthGenerator) {
	
	context = pContext;
	depth_generator = pDepthGenerator;
	
	XnStatus result = XN_STATUS_OK;	
	
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	//result = context->FindExistingNode(XN_NODE_TYPE_HANDS, handsGenerator);
	//handPos = new XnPoint3D;
	//handPosProjection = new XnPoint3D;
	result = gestureGenerator.Create(context->getXnContext());
	CHECK_RC(result, "Find gesture generator");
	result = handsGenerator.Create(context->getXnContext());
	CHECK_RC(result, "Find hand generator");
	
	// and callbacks
	XnCallbackHandle gesture_cb_handle;
	gestureGenerator.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Process, this, gesture_cb_handle);
	
	XnCallbackHandle hand_cb_handle;
	result = handsGenerator.RegisterHandCallbacks(HandCreate, HandUpdate, HandDestroy, this, hand_cb_handle);
	
	// pre-generate the tracked users.
	max_hands = MAX_NUMBER_HANDS;
	tracked_hands.reserve(max_hands);
	for(int i = 0; i < max_hands; ++i) {
		ofxTrackedHand* hand = new ofxTrackedHand(depth_generator);
		tracked_hands.push_back(hand);
	}
	found_hands = 0;
	
	isFiltering = false;
	isAcceptingGestures = false;

	// Start looking for gestures
	this->addGestures();
	ofLog(OF_LOG_VERBOSE, "Gesture camera inited");
	
	return true;
}

// Destructor
//--------------------------------------------------------------
ofxGestureGenerator::~ofxGestureGenerator()
{
	for(int i = 0; i < max_hands; ++i)
		delete tracked_hands[i];
}

//--------------------------------------------------------------
void ofxGestureGenerator::addGestures()
{
	if (!isAcceptingGestures)
	{
		gestureGenerator.AddGesture("Click", NULL);
		gestureGenerator.AddGesture("Wave", NULL);
		gestureGenerator.AddGesture("RaiseHand", NULL);
		isAcceptingGestures = true;
	}
}

//--------------------------------------------------------------
void ofxGestureGenerator::removeGestures()
{
	if (isAcceptingGestures)
	{
		gestureGenerator.RemoveGesture("Wave");
		gestureGenerator.RemoveGesture("Click");
		gestureGenerator.RemoveGesture("RaiseHand");
		isAcceptingGestures = false;
	}
}

//--------------------------------------------------------------
ofxTrackedHand* ofxGestureGenerator::getHand()
{
	// Return first tracked hand
	for(int i = 0;  i < max_hands; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->isBeingTracked)
			return hand;
	}
	return NULL;
}

// Draw all hands
//--------------------------------------------------------------
void ofxGestureGenerator::drawHands() {
	for(int i = 0;  i < max_hands; ++i)
		tracked_hands[i]->draw();
}

// Draw one hand
//--------------------------------------------------------------
void ofxGestureGenerator::drawHand(int nID) {
	for(int i = 0;  i < max_hands; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->nID == nID)
		{
			hand->draw();
			return;
		}
	}
}

// Drop all hands
//--------------------------------------------------------------
void ofxGestureGenerator::dropHands() {
	handsGenerator.StopTrackingAll();
}


//--------------------------------------------------------------
// CALLBACK PIVATES
//--------------------------------------------------------------
void ofxGestureGenerator::startHandTracking(const XnPoint3D* pPosition)
{
	handsGenerator.StartTracking(*pPosition);
}

//--------------------------------------------------------------
void ofxGestureGenerator::newHand(XnUserID nID, const XnPoint3D* pPosition)
{
	// Look for a free hand
	for(int i = 0;  i < max_hands; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->isBeingTracked == false)
		{
			hand->isBeingTracked = true;
			hand->nID = nID;
			hand->update(pPosition, isFiltering, true);
			found_hands++;
			// Stop getting gestures?
			if (found_hands >= max_hands)
				this->removeGestures();
			return;
		}
	}
}
//--------------------------------------------------------------
void ofxGestureGenerator::updateHand(XnUserID nID, const XnPoint3D* pPosition)
{
	for(int i = 0;  i < max_hands; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->nID == nID)
		{
			if (hand->isBeingTracked)
				hand->update(pPosition, isFiltering);
			return;
		}
	}
}
//--------------------------------------------------------------
void ofxGestureGenerator::destroyHand(XnUserID nID)
{
	for(int i = 0;  i < max_hands; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->nID == nID)
		{
			hand->isBeingTracked = false;
			found_hands--;
			// Resume grabbing gestures
			this->addGestures();
			return;
		}
	}
}



//--------------------------------------------------------------
//
// HANDS
//
//--------------------------------------------------------------

//--------------------------------------------------------------
ofxTrackedHand::ofxTrackedHand(ofxDepthGenerator* pDepthGenerator)
{
	depth_generator = pDepthGenerator;
	isBeingTracked = false;
}

//--------------------------------------------------------------
// High-pass filter based on iOS Accelerometer sample
// Filter: 0.01 .. 1.0
//	Lower  = less noise, less speed
//	Higher = more noise, more speed
#define FILTER_FACTOR		0.20
void ofxTrackedHand::update(const XnPoint3D* pPosition, bool filter, bool force) {
	rawPos = *pPosition;
	XnPoint3D rawProj = rawPos;
	depth_generator->getXnDepthGenerator().ConvertRealWorldToProjective(1, &rawProj, &rawProj);

	float xres = 640.0f;
	float yres = 480.0f;
	float zres = depth_generator->getMaxDepth();
	if (filter && !force)
	{
		progPos.x = ( (rawProj.X / xres) * FILTER_FACTOR) + (progPos.x * (1.0 - FILTER_FACTOR));
		progPos.y = ( (rawProj.Y / yres) * FILTER_FACTOR) + (progPos.y * (1.0 - FILTER_FACTOR));
		progPos.z = ( (rawProj.Z / zres) * FILTER_FACTOR) + (progPos.z * (1.0 - FILTER_FACTOR));
		projectPos.x = (progPos.x * xres);
		projectPos.y = (progPos.y * yres);
		projectPos.z = (progPos.z * zres);
	}
	else
	{
		projectPos = ofPoint(rawProj.X, rawProj.Y, rawProj.Z);
		progPos.x = (projectPos.x / xres);
		progPos.y = (projectPos.y / yres);
		progPos.z = (projectPos.z / zres);
	}
}

//--------------------------------------------------------------
void ofxTrackedHand::draw() {
	if (!isBeingTracked)
		return;
	ofFill();
	ofSetColor(255, 255, 0);
	ofCircle(projectPos.x, projectPos.y, 15);
}


