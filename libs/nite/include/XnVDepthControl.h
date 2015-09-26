/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEPTH_CONTROL_H_
#define _XNV_DEPTH_CONTROL_H_

#include <XnCppWrapper.h>
#include "XnVMessageListener.h"

/**
* A XnVDepthControl is a Message Listener that expects Depth Messages.
* It is meant to be a base class for Controls that are Depth-based.
*/
class XNV_NITE_API XnVDepthControl :
	public XnVMessageListener
{
public:
	/**
	* Constructor. Create a new Depth Control
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVDepthControl(const XnChar* strName = "XnVDepthControl");
	virtual ~XnVDepthControl();

	/**
	* Handle a Message. This method extracts the inner Depth Generator, and calls the other Update method.
	*
	* @param	[in]	pMessage	The message to handle
	*/
	void Update(XnVMessage* pMessage);
	/**
	* Handle a Depth Generator. This is the method users will implement.
	*
	* @param	[in]	pDG	The Depth Generator to handle
	*/
	virtual void Update(const xn::DepthGenerator* pDG) = 0;
};

#endif // _XNV_DEPTH_CONTROL_H_
