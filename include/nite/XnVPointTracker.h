/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_TRACKER_H_
#define _XNV_POINT_TRACKER_H_

#include "XnVDeviceFilter.h"
#include "XnVMultipleHands.h"

class XnVIntList;

/**
* A XnVPointTracker is a Context Filter, which is a Context Control and a Message Generator.
* It receives Context Messages (as a Context Control), and creates Point Messages to send on
*/
class XNV_NITE_API XnVPointTracker :
	public XnVContextFilter
{
public:
	/**
	* Constructor. Create a new Point Tracker
	*
	* @param	[in]	strName	A name, for Log use
	*/
	XnVPointTracker(const XnChar* strName = "XnVPointTracker");
	virtual ~XnVPointTracker();
	/**
	* Start tracking, with the first hand at a certain position
	*
	* @param	[in]	ptPosition	The initial position of the first hand
	*/
	virtual XnStatus StartTracking(const XnPoint3D& ptPosition);
	/**
	* Stop tracking a certain hand
	*
	* @param	[in]	nID	The ID of the hand that should no longer be tracked
	*/
	virtual XnStatus StopTracking(const XnUInt32 nID);
	/**
	* Stop tracking all existing hands
	*/
	virtual XnStatus StopTracking();

	/**
	* Clear state
	*/
	virtual void Clear();

protected:
	XnVIntList* m_pLostPoints;
};

#endif

