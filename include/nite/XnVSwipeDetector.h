/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SWIPE_DETECTOR_H_
#define _XNV_SWIPE_DETECTOR_H_

#include "XnVNiteDefs.h"
#include "XnVPointControl.h"
#include "XnVDirection.h"

#include "XnVSteadyDetector.h"

class XnVPointBuffer;

/**
* A control that identifies swipes - moves on the x and y axes.
* The XnVSwipeDetector defines 4 events for specific direction swipes:
* - A swipe up identified
* - A swipe down identified
* - A swipe left identified
* - A swipe right identified
*
* Additionally, a general swipe event is defined, and is called (with a XnVDirection parameter) for any swipe.
*/
class XNV_NITE_API XnVSwipeDetector :
	public XnVPointControl
{
public:
	/**
	* Type for all swipe events callbacks
	*/
	typedef void (XN_CALLBACK_TYPE *SwipeCB)(XnFloat fVelocity, XnFloat fAngle, void* pUserCxt);
	typedef void (XN_CALLBACK_TYPE *GeneralSwipeCB)(XnVDirection eDir, XnFloat fVelocity, XnFloat fAngle, void* pUserCxt);

	/**
	 * Creation
	 *
	 * @param	[in]	bUseSteadyBeforeSwipe	Wait for steady before looking for swipes
	 * @param	[in]	strName					Name of the control, for log purposes.
	 */
	XnVSwipeDetector(XnBool bUseSteadyBeforeSwipe = true, const XnChar* strName = "XnVSwipeDetector");
	~XnVSwipeDetector();

	/**
	* Called when the primary point is created.
	*
	* @param	[in]	pContext	The hand context of the newly created primary point
	* @param	[in]	ptFocus		The point in which the session has started.
	*/
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptFocus);
	/**
	* Called when the primary point is updated.
	* This will cause the algorithm to look for swipes.
	*
	* @param	[in]	pContext	The hand context of the updated primary point
	*/
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);

	/**
	* Start looking for swipes, forgetting the preexisting state.
	*/
	void Reset();

	// Events
	/**
	* Register for the swipe up event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterSwipeUp(void* cxt, SwipeCB pCB); // Add CB to list
	/**
	* Register for the swipe down event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterSwipeDown(void* cxt, SwipeCB pCB); // Add CB to list
	/**
	* Register for the swipe left event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterSwipeLeft(void* cxt, SwipeCB pCB); // Add CB to list
	/**
	* Register for the swipe right event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterSwipeRight(void* cxt, SwipeCB pCB); // Add CB to list
	/**
	* Register for the general swipe event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterSwipe(void* cxt, GeneralSwipeCB pCB);

	/**
	* Unregister from the swipe up event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterSwipeUp(XnCallbackHandle handle);
	/**
	* Unregister from the swipe down event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterSwipeDown(XnCallbackHandle handle);
	/**
	* Unregister from the swipe left event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterSwipeLeft(XnCallbackHandle handle);
	/**
	* Unregister from the swipe right event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterSwipeRight(XnCallbackHandle handle);
	/**
	* Unregister from the general swipe event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterSwipe(XnCallbackHandle handle);

	/**
	* Change the minimal speed to recognize as a swipe. Default is 0.25m/s
	* 
	* @param	[in]	fThreshold	The minimal speed to be considered a swipe
	*/
	void SetMotionSpeedThreshold(XnFloat fThreshold);
	XnFloat GetMotionSpeedThreshold() const;

	/**
	* Change the minimal duration to recognize as swipe. Default is 350 ms
	*
	* @param	[in]	nThreshold	The minimal duration to be considered a swipe
	*/
	void SetMotionTime(XnUInt32 nThreshold);
	XnUInt32 GetMotionTime() const;

	/**
	 * Change the angle difference from the X-Axis to consider Left/Right movement. Default is 25 degrees
	 *
	 * @param	[in]	fThreshold	The angle difference
	 */
	void SetXAngleThreshold(XnFloat fThreshold);
	XnFloat GetXAngleThreshold() const;

	/**
	* Change the angle difference from the Y-Axis to consider Up/Down movement. Default is 20 degrees
	*
	* @param	[in]	fThreshold	The angle difference
	*/
	void SetYAngleThreshold(XnFloat fThreshold);
	XnFloat GetYAngleThreshold() const;

	/**
	 * Change internal Steady Detector's maximum velocity
	 *
	 * @param	[in]	fVelocity	Maximum velocity to consider steady state
	 */
	void SetSteadyMaxVelocity(XnFloat fVelocity);
	XnFloat GetSteadyMaxVelocity() const;

	/**
	 * Change internal Steady Detector's steady detection duration
	 *
	 * @param	[in]	nDuration	Duration of steady state
	 */
	void SetSteadyDuration(XnUInt32 nDuration);
	XnUInt32 GetSteadyDuration() const;

	void SetUseSteady(XnBool bUse);
	XnBool GetUseSteady() const;
protected:
	XN_DECLARE_EVENT_3ARG(XnVGeneralSwipeSpecificEvent, XnVGeneralSwipeEvent, XnVDirection, eDir, XnFloat, fVelocity, XnFloat, fAngle);

	XnStatus AddPoint(const XnPoint3D& pt, XnFloat fTime);

	static void XN_CALLBACK_TYPE Steady_Steady(XnFloat fVelocity, void* cxt);

	XnVPointBuffer* m_pMovementDetectionBuffer;

	static const XnFloat ms_fDefaultMotionDetectionVelocity;	// = 0.25f
	static const XnUInt32 ms_nDefaultMotionDetectionDuration;	// = 350 ms

	XnFloat m_fMotionDetectionSpeed;
	XnUInt32 m_nMotionDetectionTime;

	XnFloat m_fLowestVelocity;
	XnBool m_bWaitingForSlowdown;

	XnFloat m_fAngleXThreshold;
	XnFloat m_fAngleYThreshold;

	XnVFloatFloatSpecificEvent* m_pPendingEvent;
	XnVDirection m_ePendingDirection;
	XnFloat m_fPendingVelocity;
	XnFloat m_fPendingAngle;

	XnVFloatFloatSpecificEvent m_SwipeDownCBs, m_SwipeUpCBs, m_SwipeLeftCBs, m_SwipeRightCBs;

	XnVGeneralSwipeSpecificEvent m_SwipeCBs;

	XnBool m_bUseSteady;
	XnBool m_bInSteady;
	XnVSteadyDetector m_Steady;
}; // XnVSwipeDetector

#endif // _XNV_SWIPE_DETECTOR_H_
