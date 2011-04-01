/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SLIDER_3D_H_
#define _XNV_SLIDER_3D_H_

#include "XnVNiteDefs.h"
#include "XnVNiteEvents.h"

/**
* This is a simple 3D slider. It receives a point,
* and normalizes it in the 3D space to numbers between 0 and 1 for each axis.
* The XnVSlider2D defines one event:
* - Its values have changed
*/
class XNV_NITE_API XnVSlider3D
{
public:
	/**
	* Type for the value change event callback. The values are between 0 and 1.
	*/
	typedef void (XN_CALLBACK_TYPE *ValueChangeCB)(XnFloat fXValue, XnFloat fYValue, XnFloat fZValue, void* pUserCxt);

	/**
	* Creation. It receives 2 points, defining the 3D space.
	*/
	XnVSlider3D(const XnPoint3D& ptMin ,const XnPoint3D& ptMax);
	
	~XnVSlider3D();
	
	/**
	* The main function. It receives a point, normalizes it, and calculates the new values.
	*
	* @param	[in]	pt	The point to check in the slider.
	*/
	XnStatus Update(const XnPoint3D& pt);

	/**
	* Register for the value change event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterValueChange(void* cxt, ValueChangeCB CB); // Add CB to list
	/**
	* Unregister from the value change event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterValueChange(XnCallbackHandle hCB);
protected:
	XN_DECLARE_EVENT_3ARG(XnVValueChange3DSpecificEvent, XnVValueChange3DEvent, XnFloat, fValueX, XnFloat, fValueY, XnFloat, fValueZ);

	// Event broadcast
	void ValueChange(XnFloat fXValue, XnFloat fYValue, XnFloat fZValue); // go over list and call all functions

	XnPoint3D m_ptMin, m_ptMax;
	XnFloat m_fSizeX, m_fSizeY, m_fSizeZ;

	XnVValueChange3DSpecificEvent m_ValueChangeCBs;
}; // XnVSlider3D

#endif // _XNV_SLIDER_3D_H_
