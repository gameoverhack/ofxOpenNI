/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_GESTURE_H_
#define _XNV_GESTURE_H_
#include <XnOpenNI.h>
#include "XnVDeviceControl.h"

class XnVGestureRecognizeSpecificEvent;
class XnVGestureStartRecognizeSpecificEvent;

/**
* A XnVGesture is a Context Control, which identifies gestures. 
*/
class XNV_NITE_API XnVGesture :
	public XnVContextControl
{
public:
	/**
	* Type for a recognized Gesture.
	* It includes the ID of the recognized gesture,
	* the position in which it was identified and the position in which it ended.
	*/
	typedef void (XN_CALLBACK_TYPE* GestureRecognizedCB)(const XnChar* strGesture, const XnPoint3D& ptIDPosition, const XnPoint3D& ptEndPosition, void* cxt);
	/**
	* Type for callbacks that a gesture started to be recognized.
	*/
	typedef void (XN_CALLBACK_TYPE* GestureStartRecognizedCB)(const XnChar* strGesture, const XnPoint3D& ptPosition, XnFloat fProgress, void* cxt);

	/**
	* Constructor. Create a new Gesture
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVGesture(const XnChar* strName = "XnVGesture");
	virtual ~XnVGesture();

	/**
	* Start looking for a gesture, in a specific area (in Real-World coordinates)
	*
	* @param	[in]	pbbArea	The area in which to look for the gesture. NULL means all over
	*/
	virtual void StartGesture(XnBoundingBox3D* pbbArea = NULL) {}
	/**
	* Stop looking for the gesture
	*/
	virtual void StopGesture() {}

	/**
	* Call all callbacks registered to the 'Gesture Recognized' event
	*
	* @param	[in]	strGesture	The name of the recognized gesture
	* @param	[in]	ptIDPosition	The position in which the gesture was recognized
	* @param	[in]	ptEndPosition	The position in which the gesture has ended
	*/
	void Recognized(const XnChar* strGesture, const XnPoint3D& ptIDPosition, const XnPoint3D& ptEndPosition);
	/**
	* Call all callbacks registered to the 'Gesture Started' event
	*
	* @param	[in]	strGesture	The name of the started gesture
	* @param	[in]	ptPosition	The position in which the gesture started
	* @param	[in]	fProgress	How much of the gesture was already completed
	*/
	void StartRecognized(const XnChar* strGesture, const XnPoint3D& ptPosition, XnFloat fProgress);

	/**
	* Register a callback to the 'Gesture Recognized' event
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterRecognize(void* cxt, GestureRecognizedCB CB);
	/**
	* Register a callback to the 'Gesture Started' event
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterStartRecognize(void* cxt, GestureStartRecognizedCB CB);

	/**
	* Unregister a callback from the 'Gesture Recognized' event
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterRecognize(XnCallbackHandle hCB);
	/**
	* Unregister a callback from the 'Gesture Started' event
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterStartRecognize(XnCallbackHandle hCB);
private:
	XnVGestureStartRecognizeSpecificEvent* m_pStartRecognizeCBs;
	XnVGestureRecognizeSpecificEvent* m_pRecognizeCBs;
};

#endif
