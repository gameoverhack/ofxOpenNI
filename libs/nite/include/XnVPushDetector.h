/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_PUSH_DETECTOR_H_
#define _XNV_PUSH_DETECTOR_H_

#include "XnVNiteDefs.h"
#include "XnVPointControl.h"
#include "XnV3DVector.h"

class XnVPointBuffer;
class XnVFloatFloatSpecificEvent;
class XnVFloatSpecificEvent;
/**
 * A control that identifies pushes - moves in the z axis.
 * The XnVPushDetector defines 2 events:
 * - A push was identified
 * - Stable state restored
 */
class XNV_NITE_API XnVPushDetector :
	public XnVPointControl
{
public:
	/**
	* Type for push event callback
	*/
	typedef void (XN_CALLBACK_TYPE *PushCB)(XnFloat fVelocity, XnFloat fAngle, void* UserCxt);
	/**
	* Type for stable event callback
	*/
	typedef void (XN_CALLBACK_TYPE *StabilizedCB)(XnFloat fVelocity, void* UserCxt);

	/**
	 * Construction.
	 *
	 * @param	[in]	strName			Name of the control, for log purposes.
	 */
	XnVPushDetector(const XnChar* strName = "XnVPushDetector");
	~XnVPushDetector();

	/**
	 * Handle the creation of the primary point
	 *
	 * @param	[in]	pContext	The hand context of the newly created primary point
	 * @param	[in]	ptFocus		The point in which the session has started.
	 */
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptFocus);
	/**
	 * called when the primary point is updated.
	 * This will cause the algorithm to look for pushes.
	 *
	 * @param	[in]	pContext	The hand context of the updated primary point
	 */
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);

	/**
	* Register to the push event
	*
	* @param	[in]	cxt	The User's context
	* @param	[in]	pCB	the callback to be called when the event is invoked
	*
	* @return	A handle, to allow unregistration
	*/
	XnCallbackHandle RegisterPush(void* cxt, PushCB pCB);
	/**
	* Register to the stable event
	*
	* @param	[in]	cxt	The User's context
	* @param	[in]	pCB	the callback to be called when the event is invoked
	*
	* @return	A handle, to allow unregistration
	*/
	XnCallbackHandle RegisterStabilized(void* cxt, StabilizedCB pCB);

	/**
	* Unregister from the push event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterPush(XnCallbackHandle handle);
	/**
	* Unregister from the stable event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterStabilized(XnCallbackHandle handle);

	/**
	* The number of frames it takes to recognize the push.
	* Effectively, this means the actual push started this number of frames ago.
	*/
	XnUInt32 GetPushDuration() const;

	/**
	* Start push detection over from scratch.
	*/
	void Reset();

	/**
	* Get the minimum velocity in the time span to define as push, in m/s
	*
	* @return the velocity
	*/
	XnFloat GetPushImmediateMinimumVelocity() const;
	/**
	* Get the time used to detect push, in ms
	*
	* @return the time
	*/
	XnUInt32 GetPushImmediateDuration() const;
	/**
	 * Get the the immediate timespan to skip, in ms
	 *
	 * @return the time
	 */
	XnUInt32 GetPushImmediateOffset() const;

	/**
	* Get the minimum velocity of the previous time span, in m/s
	*
	* @return the velocity
	*/
	XnFloat GetPushPreviousMinimumVelocity() const;
	/**
	* Get the time of the previous time span, in ms
	*
	* @return the time
	*/
	XnUInt32 GetPushPreviousDuration() const;
	/**
	* Get the time to skip of the previous time span, in ms
	*
	* @return the time
	*/
	XnUInt32 GetPushPreviousOffset() const;

	/**
	 * Get the maximum angle between the immediate direction and the Z-axis, in degrees
	 *
	 * @return the angle
	 */
	XnFloat GetPushMaximumAngleBetweenImmediateAndZ() const;
	/**
	* Get the maximum angle between the immediate direction and the previous direction, in degrees
	*
	* @return the angle
	*/
	XnFloat GetPushMinimumAngleBetweenImmediateAndPrevious() const;
	
	/**
	* Get the maximum velocity in the time span to define as stable, in m/s
	*
	* @return the velocity
	*/
	XnFloat GetStableMaximumVelocity() const;
	/**
	* Get the time used to detect stable state, in ms
	*
	* @return the time
	*/
	XnUInt32 GetStableDuration() const;

	/**
	* Change the minimum velocity in the time span to define as push. Default is 0.33 m/s
	*
	* @return the velocity
	*/
	void SetPushImmediateMinimumVelocity(XnFloat fVelocity);
	/**
	* Change the time used to detect push. Default is 240ms
	*
	* @return the time
	*/
	void SetPushImmediateDuration(XnUInt32 nDuration);
	/**
	* Change the the immediate timespan to skip. Default is 0ms
	*
	* @return the time
	*/
	void SetPushImmediateOffset(XnUInt32 nOffset);

	/**
	* Change the minimum velocity of the previous time span. Default is 0.17 m/s
	*
	* @return the velocity
	*/
	void SetPushPreviousMinimumVelocity(XnFloat fVelocity);
	/**
	* Change the time of the previous time span. Default is 150 ms
	*
	* @return the time
	*/
	void SetPushPreviousDuration(XnUInt32 nDuration);
	/**
	* Change the time to skip of the previous time span. Default is 240ms
	*
	* @return the time
	*/
	void SetPushPreviousOffset(XnUInt32 nOffset);

	/**
	* Change the maximum angle between the immediate direction and the Z-axis. Default is 30 degrees
	*
	* @return the angle
	*/
	void SetPushMaximumAngleBetweenImmediateAndZ(XnFloat fAngle);
	/**
	* Change the maximum angle between the immediate direction and the previous direction. Default is 20 degrees
	*
	* @return the angle
	*/
	void SetPushMinimumAngleBetweenImmediateAndPrevious(XnFloat fAngle);

	/**
	* Change the maximum velocity in the time span to define as stable. Default is 0.13 m/s
	*
	* @return the velocity
	*/
	void SetStableMaximumVelocity(XnFloat fVelocity);
	/**
	* Get the time used to detect stable state. Default is 360 ms
	*
	* @return the time
	*/
	void SetStableDuration(XnUInt32 nDuration);
protected:
	XnBool IsPushDetected(const XnV3DVector& vImmediateVelocity, const XnV3DVector& vPreviousVelocity, XnFloat& fZAngle);
	XnBool IsStabilized(XnFloat fTime, XnFloat& fVelocity);
	void UpdateLines(XnFloat fTime);

	void AddPoint(const XnPoint3D& pt, XnFloat fTime);

	XnFloat AngleBetweenVectors(const XnV3DVector& v1, const XnV3DVector& v2);

	XnBool m_bPushDetected;

	XnVPointBuffer* m_pPoints;

	XnFloat m_fPushImmediateMinVelocity;
	XnUInt32 m_nPushImmediateOffset;
	XnUInt32 m_nPushImmediateDuration;

	XnFloat m_fPushPreviousMinVelocity;
	XnUInt32 m_nPushPreviousOffset;
	XnUInt32 m_nPushPreviousDuration;

	XnFloat m_fPushMaxAngleFromZ;
	XnFloat m_fPushMinAngleImmediateAndPrevious;

	XnFloat m_fStableMaxVelocity;
	XnUInt32 m_nStableDuration;

	// CBs
	void PushDetected(XnFloat fVelocity, XnFloat fAngle);
	void StabilizedDetected(XnFloat fVelocity);

	XnVFloatFloatSpecificEvent* m_pPushCBs;
	XnVFloatSpecificEvent* m_pStabilizedCBs;
}; // XnVPushDetector

#endif // _XNV_PUSH_DETECTOR_H_
