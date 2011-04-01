/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_FILTER_H_
#define _XNV_FILTER_H_

#include "XnVMessageGenerator.h"
#include "XnVMessageListener.h"

/**
* A XnVFilter is a Message Listener and Generator.
* It receives Messages (as a Listener), may manipulate them, and send them (or others) on (as a Generator)
*/
class XNV_NITE_API XnVFilter :
	public XnVMessageListener,
	public XnVMessageGenerator
{
public:
	/**
	* Constructor. Create a new filter
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVFilter(const XnChar* strName = "XnVFilter");

	/**
	* Clear the Multi-threading queue
	*/
	void ClearQueue();
};

#endif
