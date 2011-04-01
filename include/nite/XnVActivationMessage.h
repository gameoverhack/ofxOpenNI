/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_ACTIVATION_MESSAGE_H_
#define _XNV_ACTIVATION_MESSAGE_H_

#include <XnOpenNI.h>

#include "XnVCCMessage.h"

/**
* A XnVActivationMessage is a Control Message holding a boolean value, indicating activate/deactivate
*/
class XNV_NITE_API XnVActivationMessage :
	public XnVCCMessage
{
public:
	/**
	* Constructor. Create a Activation Message with a boolean
	*
	* @param	[in]	bActivate	Indication of activate or deactivate
	*/
	XnVActivationMessage(XnBool bActivate);
	~XnVActivationMessage();

	/**
	* The default type for Activation Messages - "Activation"
	*/
	static const XnChar* ms_strType;
};

#endif //_XNV_ACTIVATION_MESSAGE_H_
