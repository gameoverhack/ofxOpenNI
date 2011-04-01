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
	typedef void (XN_CALLBACK_TYPE *SteadyCB)(XnFloat fVelocity, void* pUserCxt);

	/**
	* Constructor
	*
	* @param	[in]	nCooldownFrames			Minimal number of frames after input start that steady is valid
	* @param	[in]	nDetectionDuration		Minimal number of frames to constitute steady
	* @param	[in]	fMaximumAllowedVelocity	Velocity that is considered 'steady'
	* @param	[in]	strName					Name of the control, for log purposes.
	*/
	XnVSteadyDetector(XnUInt32 nCooldownFrames = ms_nDefaultInitialCooldown,
		XnUInt32 nDetectionDuration = ms_nDefaultDetectionDuration,
		XnFloat fMaximumAllowedVelocity = ms_fDefaultMaximumAllowedVelocity,
		const XnChar* strName = "XnVSteadyDetector");

	~XnVSteadyDetector();

	/**
	* Called when the primary point is created.
	*
	* @param	[in]	cxt			The hand context of the newly created primary point
	* @param	[in]	ptFocus		The point in which the session has started.
	*/
	void OnPrimaryPointCreate(const XnVHandPointContext* cxt, const XnPoint3D& ptFocus);
	/**
	* Called when the primary point is updated.
	* This will cause the algorithm to look for steady condition.
	*
	* @param	[in]	cxt	The hand context of the updated primary point
	*/
	void OnPrimaryPointUpdate(const XnVHandPointContext* cxt);

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
	* Unregister from the selection event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterSteady(XnCallbackHandle hCB);

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
	 * Get the maximum velocity in the time span to define as steady, in m/s
	 *
	 * @return the velocity
	 */
	XnFloat GetMaximumVelocity() const;

	/**
	 * Change the time it takes to detect steady state.
	 *
	 * @param	[in]	nDuration	The time it takes to detect steady state, in milliseconds (default is 200 ms)
	 */
	void SetDetectionDuration(XnUInt32 nDuration);
	/**
	 * Change the velocity that is considered steady state
	 *
	 * @param	[in]	fVelocity	The maximum velocity considered as steady state, in m/s (default is 0.01 m/s)
	 */
	void SetMaximumVelocity(XnFloat fVelocity);

	static const XnUInt32 ms_nDefaultDetectionDuration;		// = 200 ms
	static const XnUInt32 ms_nDefaultInitialCooldown;		// = 0
	static const XnFloat ms_fDefaultMaximumAllowedVelocity; // = 0.01 m/s
protected:
	// called whenever we have a new point
	XnStatus DetectSteady(const XnPoint3D& pt, XnFloat fTime);

	// broadcasts the event to all listeners
	virtual void OnSteadyDetected(XnFloat fVelocity);

	XnUInt32 m_nDetectionDuration;
	XnFloat m_fMaximumAllowedVelocity;
	XnFloat m_fMaximumAllowedVelocity2;

	XnUInt32 m_nCurrentCooldownFrames;
	XnUInt32 m_nInitialCooldownFrames;

	XnVPointBuffer* m_pPoints;

private:
	XnVFloatSpecificEvent m_SteadyCBs;
}; // XnVSteadyDetector

#endif // _XNV_STEADY_DETECTOR_H_
