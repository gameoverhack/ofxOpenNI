/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_WAVE_DETECTOR_H_
#define _XNV_WAVE_DETECTOR_H_

#include "XnVNiteDefs.h"
#include "XnVPointControl.h"

struct XnVWaveContext;
class XnVEvent;

class XNV_NITE_API XnVWaveDetector :
	public XnVPointControl
{
public:
	/**
	* Type for the wave event callbacks
	*/
	typedef void (XN_CALLBACK_TYPE *WaveCB)(void* pUserCxt);

	/**
	* Create the wave control
	*
	* @param	[in]	strName			Name of the control, for log purposes.
	*/
	XnVWaveDetector(const XnChar* strName = "XnVWaveDetector");
	~XnVWaveDetector();

	/**
	* Called when the primary point is created.
	*
	* @param	[in]	pContext	The hand context of the newly created primary point
	* @param	[in]	ptFocus		The point in which the session has started.
	*/
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptFocus);
	/**
	* Called when the primary point is updated.
	* This will cause the algorithm to look for waves.
	*
	* @param	[in]	pContext	The hand context of the updated primary point
	*/
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);
	/**
	* Called when the primary point is replaced by another.
	* This will reinitialize the algorithm for the new hand
	*
	* @param	[in]	nOldId		The ID of the old primary point
	* @param	[in]	pContext	The hand context of the new primary point
	*/
	void OnPrimaryPointReplace(XnUInt32 nOldId, const XnVHandPointContext* pContext);
	/**
	* Called when the primary point is destroyed
	*
	* @param	[in]	nID			The ID of the hand that was the primary point
	*/
	void OnPrimaryPointDestroy(XnUInt32 nID);

	/**
	* Register for the wave event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterWave(void* cxt, WaveCB pCB);
	/**
	* Unregister from the wave event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterWave(XnCallbackHandle handle);

	/**
	* Reset the algorithm.
	*/
	void Reset();

	void SetFlipCount(XnInt32 nFlipCount);
	void SetMinLength(XnInt32 nMinLength);
	void SetMaxDeviation(XnInt32 nMaxDeviation);

	XnInt32 GetFlipCount() const;
	XnInt32 GetMinLength() const;
	XnInt32 GetMaxDeviation() const;
protected:
	XnVWaveContext* m_pContext;
	XnVEvent* m_pWaveCBs;
}; // XnVWaveDetector

#endif // _XNV_WAVE_DETECTOR_H_
