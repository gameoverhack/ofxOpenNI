/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEVICE_CONTROL_H_
#define _XNV_DEVICE_CONTROL_H_

#include <XnCppWrapper.h>
#include "XnVMessageListener.h"

/**
* A XnVContextControl is a Message Listener that expects Context Messages.
* It is meant to be a base class for Controls that are Context-based.
*/
class XNV_NITE_API XnVContextControl :
	public XnVMessageListener
{
public:
	/**
	* Constructor. Create a new Context Control
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVContextControl(const XnChar* strName = "XnVContextControl");
	virtual ~XnVContextControl();

	/**
	* Handle a Message. This method extracts the inner Context, and calls the other Update method.
	*/
	void Update(XnVMessage* pMessage);
	/**
	* Handle a Context. This is the method users will implement.
	*/
	virtual void Update(const xn::Context* pContext) = 0;
};


#endif // _XNV_DEVICE_CONTROL_H_
