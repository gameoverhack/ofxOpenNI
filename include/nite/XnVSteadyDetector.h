/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_STEADY_DETECTOR_H_
#define _XNV_STEADY_DETECTOR_H_

#include "XnVNiteDefs.h"
#include "XnVPointControl.h"
class XnVPointBuffer;
class XnVSteadyStates;
class XnVUintFloatSpecificEvent;
/**
* A control that identifies return to steady condition.
* The XnVSteadyDetector defines one event:
* - Steady condition reached
*/
class XNV_NITE_API XnVSteadyDetector :
	public XnVPointControl
{
public:
	/**
	* Type for the steady event callback
	*/
	typedef void (XN_CALLBACK_TYPE *SteadyCB)(XnUInt32 nId, XnFloat fStdDev, void* pUserCxt);
	typedef void (XN_CALLBACK_TYPE *NotSteadyCB)(XnUInt32 nId, XnFloat fStdDev, void* pUserCxt);

	/**
	* Constructor
	*
	* @param	[in]	nCooldownFrames			Minimal number of frames after input start that steady is valid
	* @param	[in]	nDetectionDuration		Minimal number of frames to constitute steady
	* @param	[in]	fMaximumStdDevForSteady	Standard deviation of points that is considered 'steady'
	* @param	[in]	strName					Name of the control, for log purposes.
	*/
	XnVSteadyDetector(XnUInt32 nCooldownFrames = ms_nDefaultInitialCooldown,
		XnUInt32 nDetectionDuration = ms_nDefaultDetectionDuration,
		XnFloat fMaximumStdDevForSteady = ms_fDefaultMaximumStdDevForSteady,
		const XnChar* strName = "XnVSteadyDetector");

	~XnVSteadyDetector();
	/**
	* Register for the steady event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterSteady(void* cxt, SteadyCB CB);
	/**
	* Unregister from the steady event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterSteady(XnCallbackHandle hCB);

	/**
	 * Register for the not-steady event
	 *
	 * @param	[in]	cxt	User's context
	 * @param	[in]	CB	The callback to call when the event is invoked.
	 *
	 * @return	A handle, to allow unregistration
	 */
	XnCallbackHandle RegisterNotSteady(void* cxt, NotSteadyCB CB);
	/**
	 * Unregister from the not-steady event
	 *
	 * @param	[in]	hCB	The handle provided on registration.
	 */
	void UnregisterNotSteady(XnCallbackHandle hCB);
	/**
	* Forget the existing state, and start looking for steady state all over again.
	*/
	void Reset();

	/**
	 * Get the time used to detect steady state, in ms
	 *
	 * @return the time
	 */
	XnUInt32 GetDetectionDuration() const;
	/**
	 * Get the maximum standard deviation in the time span to define as steady, in m/s
	 *
	 * @return the standard deviation
	 */
	XnFloat GetMaximumStdDevForSteady() const;
	/**
	* Get the inximum standard deviation in the time span to define as not steady, in m/s
	*
	* @return the standard deviation
	*/
	XnFloat GetMinimumStdDevForNotSteady() const;

	/**
	 * Change the time it takes to detect steady state.
	 *
	 * @param	[in]	nDuration	The time it takes to detect steady state, in milliseconds (default is 200 ms)
	 */
	void SetDetectionDuration(XnUInt32 nDuration);
	/**
	 * Change the standard deviation that is considered steady state
	 *
	 * @param	[in]	fStdDev	The maximum standard deviation considered as steady state, in m/s (default is 0.01 m/s)
	 */
	void SetMaximumStdDevForSteady(XnFloat fStdDev);
	/**
	* Change the standard deviation that is considered not steady state
	*
	* @param	[in]	fStdDev	The minimum standard deviation considered as not steady state, in m/s (default is 0.02 m/s)
	*/
	void SetMinimumStdDevForNotSteady(XnFloat fStdDev);

	static const XnUInt32 ms_nDefaultDetectionDuration;		// = 200 ms
	static const XnUInt32 ms_nDefaultInitialCooldown;		// = 0
	static const XnFloat ms_fDefaultMaximumStdDevForSteady;	// 0.01 m/s
	static const XnFloat ms_fDefaultMinimumStdDevForNotSteady;	// 0.02 m/s

	/**
	* Called when a point is created.
	*
	* @param	[in]	cxt			The hand context of the newly created point
	*/
	void OnPointCreate(const XnVHandPointContext* cxt);
	/**
	* Called when a point is updated.
	* This will cause the algorithm to look for steady condition for this hand
	*
	* @param	[in]	cxt	The hand context of the updated point
	*/
	void OnPointUpdate(const XnVHandPointContext* cxt);

	XnFloat XN_API_DEPRECATED("Please use GetMaximumStdDevForSteady() instead.")
		GetMaximumVelocity() const;
	void XN_API_DEPRECATED("Please use SetMaximumStdDevForSteady() instead.")
		SetMaximumVelocity(XnFloat fVelocity);
protected:
	void Reset(XnUInt32 id);
	// called whenever we have a new point
	XnStatus DetectSteady(XnUInt32 nId, const XnPoint3D& pt, XnFloat fTime);

	// broadcasts the event to all listeners
	virtual void OnSteadyDetected(XnUInt32 nId, XnFloat fStdDev);
	virtual void OnNotSteadyDetected(XnUInt32 nId, XnFloat fStdDev);

	XnUInt32 m_nDetectionDuration;
	XnFloat m_fMaximumStdDevForSteady;
	XnFloat m_fMaximumVarianceForSteady;
	XnFloat m_fMinimumStdDevForNotSteady;
	XnFloat m_fMinimumVarianceForNotSteady;

	XnUInt32 m_nInitialCooldownFrames;

	XnVSteadyStates* m_pSteadyStates;
private:
	XnVUintFloatSpecificEvent* m_pSteadyCBs;
	XnVUintFloatSpecificEvent* m_pNotSteadyCBs;
}; // XnVSteadyDetector

#endif // _XNV_STEADY_DETECTOR_H_
