/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SLIDER_2D_H_
#define _XNV_SLIDER_2D_H_

#include "XnVNiteDefs.h"
#include "XnVNiteEvents.h"
#include "XnVDirection.h"

class XnVPointBuffer;

/**
* This is a simple 2D slider. It receives a point,
* and normalizes it in the x-y 2D space to numbers between 0 and 1 for each.
* The XnVSlider2D defines 2 events:
* - Its values have changed
* - A movement in the z-coordinate was detected.
*/
class XNV_NITE_API XnVSlider2D
{
public:
	/**
	* Type for the value change event callback. The values are between 0 and 1 in each axis
	*/
	typedef void (XN_CALLBACK_TYPE *ValueChangeCB)(XnFloat fXValue, XnFloat fYValue, void* pUserCxt);
	/**
	* Type for the off axis movement event callback. It receives a direction of the off-axis movement.
	*/
	typedef void (XN_CALLBACK_TYPE *OffAxisMovementCB)(XnVDirection eDir, void* pUserCxt);

	/**
	* Creation. It receives 2 points, defining the x-y 2D space.
	* 
	* @param	[in]	ptMin	One edge of the 2D space
	* @param	[in]	ptMax	Other edge of the 2D space
	*/
	XnVSlider2D(const XnPoint3D& ptMin ,const XnPoint3D& ptMax);
	/**
	* Creation.
	* 
	* @param	[in]	ptInitialPosition	The initial point within the slider
	* @param	[in]	fSliderXLength		The length of the slider in the x-coordinate
	* @param	[in]	fSliderYLength		The length of the slider in the y-coordinate
	* @param	[in]	fInitialXValue		The initial value to consider the initial point in the x-coordindate
	* @param	[in]	fInitialYValue		The initial value to consider the initial point in the y-coordindate
	*/
	XnVSlider2D(const XnPoint3D& ptInitialPosition, XnFloat fSliderXLength, XnFloat fSliderYLength,
				XnFloat fInitialXValue, XnFloat fInitialYValue);

	~XnVSlider2D();
	/**
	* The main function. It receives a point, normalizes it, and calculates the new values.
	*
	* @param	[in]	pt				The point to check in the slider.
	* @param	[in]	fTime			The timestamp (in seconds) for the update
	* @param	[in]	bCheckOffAxis	should check for OffAxis
	*/
	XnStatus Update(const XnPoint3D& pt, XnFloat fTime, XnBool bCheckOffAxis = true);

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
	* Register for the off-axis event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterOffAxisMovement(void* cxt, OffAxisMovementCB CB); 
	/**
	* Unregister from the value change event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterValueChange(XnCallbackHandle hCB);
	/**
	* Unregister from the off-axis event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterOffAxisMovement(XnCallbackHandle hCB);

	XnPoint3D GetPosition() const {return m_ptCurrentPosition;}

	/**
	* Get the minimum velocity to consider Off Axis movement
	*
	* @return	The minimum velocity to consider OffAxis movement.
	*/
	XnFloat  GetOffAxisDetectionVelocity() const;
	/**
	* Get the minimum angle to consider Off Axis movement
	*
	* @return The minimum angle to consider Off Axis movement
	*/
	XnFloat  GetOffAxisDetectionAngle() const;
	/**
	* Get the time span in which an off axis movement should be identified
	*
	* @return The timespan in which an off axis movement should be identified.
	*/
	XnUInt32 GetOffAxisDetectionTime() const;

	/**
	* Change the minimum velocity to consider Off Axis movement, in m/s. Default is 0.15m/s
	*
	* @param	[in]	fVelocity	New minimum velocity
	*/
	void SetOffAxisDetectionVelocity(XnFloat fVelocity);
	/**
	* Change the minimum angle to consider Off Axis movement, in degrees. Default is 60
	*
	* @param	[in]	fAngle	New minimum angle
	*/
	void SetOffAxisDetectionAngle(XnFloat fAngle);
	/**
	* Change the time span in which an Off Axis movement should be identified, in milliseconds. Default is 350ms
	*
	* @param	[in]	nTime	New time span
	*/
	void SetOffAxisDetectionTime(XnUInt32 nTime);
protected:
	XnInt32 CheckForOffAxisMovement(const XnPoint3D& pt, XnFloat fTime);

	XnPoint3D m_ptMin, m_ptMax;
	XnFloat m_fSizeX, m_fSizeY;

	XnVPointBuffer* m_pPointBuffer;

	XnPoint3D m_ptCurrentPosition;

	// Event broadcast
	void ValueChange(XnFloat fXValue, XnFloat fYValue); // go over list and call all functions
	void OffAxisMovement(XnVDirection eDir);

	XnVFloatFloatSpecificEvent m_ValueChangeCBs;
	XnVDirectionSpecificEvent m_OffAxisMovementCBs;

	static const XnFloat ms_fOffAxisDefaultMininumVelocity;	// = 0.17 m/s
	static const XnFloat ms_fOffAxisDefaultDetectionAngle;		// = 60.0f
	static const XnUInt32 ms_nDefaultTimeForOffAxisDetection;	// = 350 ms

	XnFloat m_fOffAxisDetectionVelocity;
	XnFloat m_fOffAxisDetectionAngle;
	XnUInt32 m_nOffAxisDetectionTime;
};


#endif // _XNV_SLIDER_2D_H_
