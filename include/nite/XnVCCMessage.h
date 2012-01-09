/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_CC_MESSAGE_H_
#define _XNV_CC_MESSAGE_H_

#include "XnVMessage.h"

/**
* A generic Message holding a single boolean value, basically for command and control (CC)
*/
class XNV_NITE_API XnVCCMessage :
	public XnVMessage
{
public:
	/**
	* Constructor. Create a CC Message with a boolean and a specific type
	*
	* @param	[in]	strType	The type of the C&C Message
	* @param	[in]	bValue	The internal value
	*/
	XnVCCMessage(const XnChar* strType, XnBool bValue);
	~XnVCCMessage();

	/**
	* Create a new Message of the same type (CC Message), with a clone of the same data (the same boolean value)
	*
	* @return	The new message
	*/
	XnVMessage* Clone() const;

	/**
	* Get the internal value
	*
	* @return The internal value
	*/
	XnBool GetValue() const;

	static XnStatus GetValue(XnVMessage* pMessage, const XnChar* strType, XnBool& bValue);
protected:
	XnBool m_bValue;
};

#endif