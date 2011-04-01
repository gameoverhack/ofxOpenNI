/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEVICE_MESSAGE_H_
#define _XNV_DEVICE_MESSAGE_H_

#include <XnCppWrapper.h>
#include "XnVMessage.h"

/**
 * A Message that holds a XnVDevice
 */
class XNV_NITE_API XnVContextMessage :
	public XnVMessage
{
public:
	/**
	* Constructor. Create a Context Message with a Context.
	*
	* @param	[in]	pContext	The Context to use in the Context Message
	*/
	XnVContextMessage(const xn::Context* pContext);
	~XnVContextMessage();

	/**
	* Create a new Message of the same type (Context Message), with a the same data (the same Device)
	*
	* @return	The new message
	*/
	XnVMessage* Clone() const;

	/**
	* The default type for Context Messages - "Context"
	*/
	static const XnChar* ms_strType;
protected:
	const xn::Context* m_pContext;

};

#endif