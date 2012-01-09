/*
 * ofxHandGenerator.cpp
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

#include "ofxHandGenerator.h"
#include "ofxOpenNIMacros.h"
#include "ofxTrackedHand.h"

// ctor
//--------------------------------------------------------------
ofxHandGenerator::ofxHandGenerator(){
	// set defaults
	setMinDistBetweenHands(100);
	setSmoothing(1);
	setMinTimeBetweenHands(500);
}

// dtor
//--------------------------------------------------------------
ofxHandGenerator::~ofxHandGenerator()
{
	// TODO: Unregister callbacks on shutdown
	tracked_hands.clear();
	//removeGestures();
	hands_generator.Unref();
}

//--------------------------------------------------------------
// GESTURE EVENT LISTENER CALLBACK
//--------------------------------------------------------------
void ofxHandGenerator::gestureRecognized(gesture & last_gesture) {

	XnPoint3D handPos;
	bool startTracking = false;
	
	if (found_hands == 0) {		// if we don't have any hands lets try to track
		
		startTracking = true;

	} else {
		
		startTracking = true; // do negative test by looking for the distance between each existing hand

		for (int i = 0; i < max_hands; i++) {
			
			// get raw position of this hand
			handPos = tracked_hands[i]->rawPos;
			
			// calculate distance between End ID of gesture and this hand
			float distanceToHand = sqrt( pow( last_gesture.gesture_position.x-handPos.X, 2 ) +
										 pow( last_gesture.gesture_position.y-handPos.Y, 2 ) +
										 pow( last_gesture.gesture_position.z-handPos.Z, 2 ) );
			
			printf("Hand dist: %f pos: [%d, %d, %d]\n", distanceToHand, (int)handPos.X, (int)handPos.Y, (int)handPos.Z);
			
			// if the hand is within the min distance then don't track it
			if (distanceToHand < min_distance) {	
				startTracking = false;
				continue;
			}
		}
	}
	
	if (startTracking) {
		handPos.X = last_gesture.gesture_position.x;
		handPos.Y = last_gesture.gesture_position.y;
		handPos.Z = last_gesture.gesture_position.z;
		
		startHandTracking((const XnPoint3D*)&handPos);
	}
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
bool ofxHandGenerator::setup(ofxOpenNIContext* pContext, int number_of_hands) {
	
	context = pContext;
	context->getDepthGenerator(&depth_generator);
	
	XnStatus result = XN_STATUS_OK;	
	
	// setup a gesture generator using our ofxGestureGenerator wrapper
	gesture_generator.setup(context);
	ofAddListener(gesture_generator.gestureRecognized, this, &ofxHandGenerator::gestureRecognized); 
	
	// Try to fetch hands generator before creating one
	if(pContext->getHandsGenerator(&hands_generator)) {
		// found the hands generator - for do nothing		
	} else {
		result = hands_generator.Create(context->getXnContext());
		CHECK_RC(result, "Creating hand generator");
		
		if (result != XN_STATUS_OK) return false;
		
		hands_generator.StartGenerating();
	}
	
	// pre-generate the tracked users.
	setMaxNumHands(number_of_hands);
	
	found_hands = 0;
	
	isFiltering = false;
	
	printf("Hands generator inited\n");
	
	startTrackHands();
	
	return true;
}

//--------------------------------------------------------------
void ofxHandGenerator::addGestures() {
	gesture_generator.addGesture("Click");
	gesture_generator.addGesture("Wave");
	gesture_generator.addGesture("RaiseHand");
}

//--------------------------------------------------------------
void ofxHandGenerator::removeGestures() {
	gesture_generator.removeGesture("Wave");
	gesture_generator.removeGesture("Click");
	gesture_generator.removeGesture("RaiseHand");

}

//--------------------------------------------------------------
int ofxHandGenerator::getNumTrackedHands() {
	return found_hands;
}

//--------------------------------------------------------------
void ofxHandGenerator::setSmoothing(float smooth) {
	if (smooth > 0.0f && smooth <= 1.0f) {
		smoothing_factor = smooth;
		if (hands_generator.IsValid()) {
			hands_generator.SetSmoothing(smooth);
		}
	}
}

//--------------------------------------------------------------
float ofxHandGenerator::getSmoothing() {
	return smoothing_factor;
}

//--------------------------------------------------------------
void ofxHandGenerator::setMaxNumHands(int number_of_hands) {
	
	if (number_of_hands > 0) {

		max_hands = number_of_hands; // do we want a maximum?
		cout << max_hands - tracked_hands.size() << endl;
		// check if we have enough hand trackers
		if (tracked_hands.size() < max_hands) {
			// if not add them

			int totalHandsToAdd = max_hands - tracked_hands.size();

			for(int i = 0; i < totalHandsToAdd; ++i) {
			printf("Creating hand: %d", i);
			ofxTrackedHand* hand = new ofxTrackedHand(context);
			tracked_hands.push_back(hand);
			}
		} // TODO: handle for subtracking total number of hands....
	}

}

//--------------------------------------------------------------
int ofxHandGenerator::getMaxNumHands() {
	return max_hands;
}

//--------------------------------------------------------------
void ofxHandGenerator::setMinDistBetweenHands(int distance) {
	if (distance > 0) min_distance = distance; // do we want a minimum?
}

//--------------------------------------------------------------
int ofxHandGenerator::getMinDistBetweenHands() {
	return min_distance;
}

//--------------------------------------------------------------
void ofxHandGenerator::setMinTimeBetweenHands(int time) {
	if (time > 0) {
		min_time = time;
		gesture_generator.setMinTimeBetweenGestures(min_time);
	}
}

//--------------------------------------------------------------
int ofxHandGenerator::getMinTimeBetweenHands() {
	return min_time;
}

//--------------------------------------------------------------
void ofxHandGenerator::setFilterFactors(float filter) {
	// set all filter factors to the same number
	for(int i = 0;  i < max_hands; ++i) 
		setFilterFactor(filter, i);
}

//--------------------------------------------------------------
void ofxHandGenerator::setFilterFactor(float filter, int thIndex) {
	// set one filter factor to a number
	ofxTrackedHand *hand = tracked_hands[thIndex];
	hand->setFilterFactor(filter);
}

// Get hand at nID TODO: switch from vector to array & make the look up faster (ie., direct to ID rather than cycling through IDs)
//--------------------------------------------------------------
ofxTrackedHand* ofxHandGenerator::getHand(int thIndex)
{
	// Return thIndex tracked hand
	ofxTrackedHand *hand = tracked_hands[thIndex]; // here thIndex refers to index not the actually HandGenerators nID which seems to count forever....
	//if (hand->isBeingTracked) {
		return hand;
	//}
	//return NULL;
}

// Draw all hands
//--------------------------------------------------------------
void ofxHandGenerator::drawHands() {
	for(int i = 0;  i < max_hands; ++i)
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

// Stop/Start hand tracking
//--------------------------------------------------------------
void ofxHandGenerator::toggleTrackHands() {
	if (!bIsTracking) {
		startTrackHands();
	} else stopTrackHands();
}

// Start hand tracking
//--------------------------------------------------------------
void ofxHandGenerator::startTrackHands() {
	
	dropHands();

	hands_generator.RegisterHandCallbacks(HandCreate, HandUpdate, HandDestroy, this, hand_cb_handle);
	
	// Start looking for gestures
	this->addGestures();
	
	printf("Hands generator started\n");
	
	bIsTracking = true;
	
}

// Stop hand tracking
//--------------------------------------------------------------
void ofxHandGenerator::stopTrackHands() {
	
	dropHands();
	
	hands_generator.UnregisterHandCallbacks(hand_cb_handle);
	
	printf("Hands generator stopped\n");
	
	// Start looking for gestures
	this->removeGestures();
	
	bIsTracking = false;
	
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
	for(int i = 0;  i < max_hands; ++i) {
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
void ofxHandGenerator::destroyHand(XnUserID nID)
{
	for(int i = 0;  i < max_hands; ++i) {
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
