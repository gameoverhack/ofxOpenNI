#include "ofxHandGenerator.h"
#include "ofxOpenNIMacros.h"
#include "ofxTrackedHand.h"

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
	printf("Gesture recognized: %s  posID [%d, %d, %d]  posEND [%d, %d, %d]\n", strGesture,
		   (int)pIDPosition->X, (int)pIDPosition->Y, (int)pIDPosition->Z,
		   (int)pEndPosition->X, (int)pEndPosition->Y, (int)pEndPosition->Z);
	
	ofxHandGenerator* gest = static_cast<ofxHandGenerator*>(pCookie);
	
	bool startTracking = false;
	
	if (gest->getNumberofTrackedHands() == 0) {
		startTracking = true;
	} else {
		
		startTracking = true; // do negative test by looking for the distance between each existing hand
		
		for (int i = 0; i < MAX_NUMBER_HANDS; i++) {
			
			// get raw position of this hand
			XnPoint3D handPos = gest->tracked_hands[i]->rawPos;
			
			// calculate distance between End ID of gesture and this hand
			float distanceToHand = sqrt(((int)pEndPosition->X-handPos.X)*((int)pEndPosition->X-handPos.X) +
										((int)pEndPosition->Y-handPos.Y)*((int)pEndPosition->Y-handPos.Y) +
										((int)pEndPosition->Z-handPos.Z)*((int)pEndPosition->Z-handPos.Z));
			
			printf("Hand dist: %f pos: [%d, %d, %d]\n", distanceToHand, (int)handPos.X, (int)handPos.Y, (int)handPos.Z);
			
			// if the hand is within the min distance then don't track it
			if (distanceToHand < MIN_DIST_BETWEEN_HANDS) {	
				startTracking = false;
				continue;
			}
		}
	}
	
	if (startTracking) {
		gest->startHandTracking(pEndPosition);
	}
	
	
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
	ofxHandGenerator* gest = static_cast<ofxHandGenerator*>(pCookie);
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
	ofxHandGenerator* gest = static_cast<ofxHandGenerator*>(pCookie);
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
	ofxHandGenerator* gest = static_cast<ofxHandGenerator*>(pCookie);
	gest->destroyHand(nID);
}


//--------------------------------------------------------------
ofxHandGenerator::ofxHandGenerator(){
}

//--------------------------------------------------------------
bool ofxHandGenerator::setup(ofxOpenNIContext* pContext) {
	
	pContext->getDepthGenerator(&depth_generator);
	
	XnStatus result = XN_STATUS_OK;	
	
	// Try to fetch gesture generator before creating one
	if(pContext->getGestureGenerator(&gesture_generator)) {
		// found the gesture generator - for do nothing		
	} else {
		result = gesture_generator.Create(pContext->getXnContext());
		CHECK_RC(result, "Creating gesture generator");
		gesture_generator.StartGenerating();
	}
	
	XnCallbackHandle gesture_cb_handle;
	gesture_generator.RegisterGestureCallbacks(Gesture_Recognized, Gesture_Process, this, gesture_cb_handle);
	
	printf("Gesture generator inited\n");

	// Try to fetch hands generator before creating one
	if(pContext->getHandsGenerator(&hands_generator)) {
		// found the hands generator - for do nothing		
	} else {
		result = hands_generator.Create(pContext->getXnContext());
		CHECK_RC(result, "Creating hand generator");
		hands_generator.StartGenerating();
	}
	
	XnCallbackHandle hand_cb_handle;
	hands_generator.RegisterHandCallbacks(HandCreate, HandUpdate, HandDestroy, this, hand_cb_handle);
	
	printf("Hands generator inited\n");
	
	// pre-generate the tracked users.
	tracked_hands.reserve(MAX_NUMBER_HANDS);
	for(int i = 0; i < MAX_NUMBER_HANDS; ++i) {
		ofxTrackedHand* hand = new ofxTrackedHand(pContext);
		tracked_hands.push_back(hand);
	}
	found_hands = 0;
	
	isFiltering = false;

	// Start looking for gestures
	this->addGestures();

	return true;
}

//--------------------------------------------------------------
int ofxHandGenerator::getNumberofTrackedHands() {
	return found_hands;
}

//--------------------------------------------------------------
void ofxHandGenerator::addGestures() {
	gesture_generator.AddGesture("Click", NULL);
	gesture_generator.AddGesture("Wave", NULL);
	gesture_generator.AddGesture("RaiseHand", NULL);

}

//--------------------------------------------------------------
void ofxHandGenerator::removeGestures() {
	gesture_generator.RemoveGesture("Wave");
	gesture_generator.RemoveGesture("Click");
	gesture_generator.RemoveGesture("RaiseHand");

}

// Get hand at nID TODO: switch from vector to array & make the look up faster (ie., direct to ID rather than cycling through IDs)
//--------------------------------------------------------------
ofxTrackedHand* ofxHandGenerator::getHand(int thIndex)
{
	// Return thIndex tracked hand
	ofxTrackedHand *hand = tracked_hands[thIndex]; // here thIndex refers to index not the actually HandGenerators nID which seems to count forever....
	if (hand->isBeingTracked) {
		return hand;
	}
	return NULL;
}

// Draw all hands
//--------------------------------------------------------------
void ofxHandGenerator::drawHands() {
	for(int i = 0;  i < MAX_NUMBER_HANDS; ++i)
		drawHand(i);
}

// Draw one hand
//--------------------------------------------------------------
void ofxHandGenerator::drawHand(int thIndex) {

	ofxTrackedHand *hand = tracked_hands[thIndex]; // here thIndex refers to index not the actually HandGenerators nID which seems to count forever....
	if (hand->isBeingTracked) hand->draw();

}

// Drop all hands
//--------------------------------------------------------------
void ofxHandGenerator::dropHands() {
	hands_generator.StopTrackingAll();
}


//--------------------------------------------------------------
// CALLBACK PIVATES
//--------------------------------------------------------------
void ofxHandGenerator::startHandTracking(const XnPoint3D* pPosition) {
	hands_generator.StartTracking(*pPosition);
}

//--------------------------------------------------------------
void ofxHandGenerator::newHand(XnUserID nID, const XnPoint3D* pPosition)
{
	// Look for a free hand
	for(int i = 0;  i < MAX_NUMBER_HANDS; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->isBeingTracked == false)
		{
			hand->isBeingTracked = true;
			hand->nID = nID;
			hand->update(pPosition, isFiltering, true);
			found_hands++;
			// Stop getting gestures?
			//if (found_hands >= MAX_NUMBER_HANDS)
			//	this->removeGestures();
			return;
		}
	}
}
//--------------------------------------------------------------
void ofxHandGenerator::updateHand(XnUserID nID, const XnPoint3D* pPosition)
{
	for(int i = 0;  i < MAX_NUMBER_HANDS; ++i) {
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
void ofxHandGenerator::destroyHand(XnUserID nID)
{
	for(int i = 0;  i < MAX_NUMBER_HANDS; ++i) {
		ofxTrackedHand *hand = tracked_hands[i];
		if (hand->nID == nID) 
		{
			// reset position to all 0's -> is this dodgy?
			hand->rawPos.X = 0;
			hand->rawPos.Y = 0;
			hand->rawPos.Z = 0;
			
			hand->isBeingTracked = false;
			found_hands--;
			// Resume grabbing gestures ?
			//this->addGestures();
			return;
		}
	}
}

// Destructor
//--------------------------------------------------------------
ofxHandGenerator::~ofxHandGenerator()
{
	tracked_hands.clear();
}