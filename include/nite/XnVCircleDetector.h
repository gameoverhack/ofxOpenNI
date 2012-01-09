/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_CIRCLE_DETECTOR_H_
#define _XNV_CIRCLE_DETECTOR_H_

#include "XnVPointControl.h"
#include "XnVNiteDefs.h"
#include "XnVCircle.h"

class XnVPointBuffer;
class XnVCircleSolver;

class XnVNoCircleSpecificEvent;
class XnVCircleSpecificEvent;

/**
* Identify a circle. This is done by starting from a single point, and finding points going further away,
* then back closer. A best fitting circle is found, and makes sure it really is a circle - there are points in all
* four quadrants. The circle must be a 'correct' size, its radius between a minimum and maximum allowed values.
* Once the circle exists, further points are matched to it, possibly slightly changing the center of the circle.
* Callbacks are called to inform the current position inside the circle and the number of circles complete.
* Clockwise circles are considered in a positive direction, and counter clockwise in a negative direction.
* A callback is also called when the points no longer describe a circle.
*/
class XNV_NITE_API XnVCircleDetector :
	public XnVPointControl
{
public:
	/**
	* Reasons why there would be no available circle
	*/
	enum XnVNoCircleReason
	{
		NO_CIRCLE_ILLEGAL,
		NO_CIRCLE_NO_INPUT,
		NO_CIRCLE_BAD_POINTS,
		NO_CIRCLE_MANUAL
	};

	/**
	* Type for the circle event callbacks. Gives number of circles detected, confidence of current state, and the circle itself
	* 
	* @param	[in]	fTimes		Number of circles already complete
	* @param	[in]	bConfident	Confidence of the circle
	* @param	[in]	pCircle		The circle that is used
	* @param	[in]	pUserCxt	User's context, passed in the registration
	*/
	typedef void (XN_CALLBACK_TYPE *CircleCB)(XnFloat fTimes, XnBool bConfident, const XnVCircle* pCircle, void* pUserCxt);
	/**
	* Type for the no-circle event callbacks. Gives the last value it had in the circle, and a reason.
	*
	* @param	[in]	fLastValue	Number of circles completed while the circle was detected
	* @param	[in]	eReason		A reason as to why this is no longer a circle
	* @param	[in]	pUserCxt	User's context, passed in the registration
	*/
	typedef void (XN_CALLBACK_TYPE *NoCircleCB)(XnFloat fLastValue, XnVNoCircleReason eReason, void* pUserCxt);

	/**
	 * Creation
	 *
	 * @param	[in]	strName			Name of the control, for log purposes.
	 */
	XnVCircleDetector(const XnChar* strName = "XnVCircleDetector");
	~XnVCircleDetector();

	/**
	* Called when the primary point is created.
	*
	* @param	[in]	pContext	The hand context of the newly created primary point
	* @param	[in]	ptFocus		The point in which the session has started.
	*/
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptFocus);
	/**
	* Called when the primary point is updated.
	* This will cause the algorithm to detect circles.
	*
	* @param	[in]	pContext	The hand context of the updated primary point
	*/
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);
	/**
	* Called when the primary point is destroyed.
	*
	* @param	[in]	nID	the ID of the last primary point
	*/
	void OnPrimaryPointDestroy(XnUInt32 nID);

	/**
	* Register for the circle event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterCircle(void* cxt, CircleCB pCB);
	/**
	* Register for the no-circle event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterNoCircle(void* cxt, NoCircleCB pCB);

	/**
	* Unregister from the circle event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterCircle(XnCallbackHandle handle);
	/**
	* Unregister from the no-circle event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterNoCircle(XnCallbackHandle handle);

	/**
	* Stop recognizing the circle. Will result in a NoCircle callback being called if a circle existed.
	*/
	void Reset();

	/**
	* Change the minimum number of points to consider a circle.
	*
	* @param	[in]	nMinimumPoints	New threshold. Default is 20.
	*/
	XnStatus SetMinimumPoints(XnUInt32 nMinimumPoints);
	/**
	* Change how close a point must be (in mms) to an existing circle to be considered a part of it.
	*
	* @param	[in]	fCloseToExistingRadius	New threshold. Default is 50
	*/
	XnStatus SetCloseToExistingRadius(XnFloat fCloseToExistingRadius);
	/**
	* Change how close (mms, distance squared) point should be to the original, to start considering it a circle.
	* This is after the point has gotten further away and then back closer.
	*
	* @param	[in]	fCloseEnough	New threshold. Default is 1000
	*/
	XnStatus SetCloseEnough(XnFloat fCloseEnough);
	/**
	* Change the minimal size (in mms) of a valid circle's radius. This is used when no existing circle exists.
	*
	* @param	[in]	fMinRadius	New threshold. Default is 40
	*/
	XnStatus SetMinRadius(XnFloat fMinRadius);
	/**
	* Change the maximal size (in mms) of a valid circle's radius. This is used when no existing circle exists.
	*
	* @param	[in]	fMaxRadius	New threshold. Default is 1200
	*/
	XnStatus SetMaxRadius(XnFloat fMaxRadius);
	/**
	* Change the weight of the existing circle's radius (relative to the new point)
	*
	* @param	[in]	fExistingWeight	New threshold. Default is 0.8 (which means 0.2 for the new point)
	*/
	XnStatus SetExistingWeight(XnFloat fExistingWeight);
	/**
	* Change the number of points that don't match the circle but still stay in circle mode.
	*
	* @param	[in]	nMaxErrors	New threshold. Default is 5
	*/
	XnStatus SetMaxErrors(XnUInt32 nMaxErrors);

	/**
	* Get the minimum number of points to consider a circle.
	*
	* @param	[out]	nMinimumPoints	The threshold.
	*/
	XnStatus GetMinimumPoints(XnUInt32& nMinimumPoints) const;
	/**
	* Get how close a point must be (in mms) to an existing circle to be considered a part of it.
	*
	* @param	[out]	fCloseToExistingRadius	The threshold.
	*/
	XnStatus GetCloseToExistingRadius(XnFloat& fCloseToExistingRadius) const;
	/**
	* Get how close (mms, distance squared) point should be to the original, to start considering it a circle.
	* This is after the point has gotten further away and then back closer.
	*
	* @param	[out]	fCloseEnough	The threshold.
	*/
	XnStatus GetCloseEnough(XnFloat& fCloseEnough) const;
	/**
	* Get the minimal size (in mms) of a valid circle's radius. This is used when no existing circle exists.
	*
	* @param	[out]	fMinRadius	The threshold.
	*/
	XnStatus GetMinRadius(XnFloat& fMinRadius) const;
	/**
	* Get the maximal size (in mms) of a valid circle's radius. This is used when no existing circle exists.
	*
	* @param	[out]	fMaxRadius	The threshold.
	*/
	XnStatus GetMaxRadius(XnFloat& fMaxRadius) const;
	/**
	* Get the weight of the existing circle's radius (relative to the new point)
	*
	* @param	[out]	fExistingWeight	The threshold.
	*/
	XnStatus GetExistingWeight(XnFloat& fExistingWeight) const;
	/**
	* Get the number of points that don't match the circle but still stay in circle mode.
	*
	* @param	[out]	nMaxErrors	The threshold.
	*/
	XnStatus GetMaxErrors(XnUInt32& nMaxErrors) const;

protected:
	void AddPoint(const XnPoint3D& pt, XnFloat fTime);

	XnVCircleSolver* m_pCircleSolver;
	XnVCircle* m_pCurrentCircle;

	XnFloat m_fCurrentAngle;
	XnInt32 m_nCircles;
	XnBool m_bCircleExists;
	XnUInt32 m_nErrors;

	XnUInt32 m_nMinimumPoints;
	XnFloat m_fCloseToExistingRadius;
	XnFloat m_fCloseEnough;
	XnFloat m_fMinRadius;
	XnFloat m_fMaxRadius;
	XnFloat m_fExistingWeight;
	XnFloat m_fNewRadiusWeight;
	XnUInt32 m_nMaxErrors;

	static const XnUInt32 ms_nDefaultMinimumPoints;		// = 20
	static const XnFloat ms_fDefaultCloseToExistingRadius;	// = 50
	static const XnFloat ms_fDefaultCloseEnough;			// = 1000
	static const XnFloat ms_fDefaultMinRadius;				// = 40
	static const XnFloat ms_fDefaultMaxRadius;				// = 1200
	static const XnFloat ms_fDefaultExistingWeight;		// = 0.8f
	static const XnUInt32 ms_nDefaultMaxErrors;			// = 5

	XnVPointBuffer* m_pMovementDetectionBuffer;

	XnVCircleSpecificEvent* m_pCircleCBs;
	XnVNoCircleSpecificEvent* m_pNoCircleCBs;
};

#endif // _XNV_CIRCLE_DETECTOR_H_
