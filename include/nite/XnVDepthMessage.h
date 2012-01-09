/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEPTH_MESSAGE_H_
#define _XNV_DEPTH_MESSAGE_H_

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#include "XnVMessage.h"

/**
* A XnVDepthMessage is a Message holding a Depth Generator
*/
class XNV_NITE_API XnVDepthMessage :
	public XnVMessage
{
public:
	/**
	* Constructor. Create a Depth Message with a Depth Generator.
	*
	* @param	[in]	pDG	The Depth Generator to use in the Depth Message
	*/
	XnVDepthMessage(const xn::DepthGenerator* pDG);
	~XnVDepthMessage();

	/**
	* Create a new Message of the same type (Depth Message), with a clone of the same data (the same Depth Generator)
	*
	* @return	The new message
	*/
	XnVMessage* Clone() const;

	/**
	* The default type for Depth Messages - "Depth"
	*/
	static const XnChar* ms_strType;
protected:
	const xn::DepthGenerator* m_pDG;
};

#endif // _XNV_DEPTH_MESSAGE_H_
