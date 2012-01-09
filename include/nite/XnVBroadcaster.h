/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_BROADCASTER_H_
#define _XNV_BROADCASTER_H_

#include "XnVFilter.h"

/**
* A XnVBroadcast is a specific Filter, which sends the messages it receives to its listeners, unchanged.
*/
class XNV_NITE_API XnVBroadcaster :
	public XnVFilter
{
public:
	/**
	* Constructor. Create a new Broadcaster
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVBroadcaster(const XnChar* strName = "Broadcaster");
	/**
	* Send all received messages to listeners.
	*
	* @param	[in]	pMessage	The message to handle
	*/
	void Update(XnVMessage* pMessage);
};

#endif // _XNV_BROADCASTER_H_
