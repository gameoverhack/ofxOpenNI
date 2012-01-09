/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_IMAGE_MESSAGE_H_
#define _XNV_IMAGE_MESSAGE_H_

#include <XnOpenNI.h>
#include <XnCppWrapper.h>

#include "XnVMessage.h"

/**
* A XnVImageMessage is a Message holding an Image Generator
*/
class XNV_NITE_API XnVImageMessage :
	public XnVMessage
{
public:
	/**
	* Constructor. Create an Image Message with an Image Generator.
	*
	* @param	[in]	pIG	The Image Generator to use in the Image Message
	*/
	XnVImageMessage(const xn::ImageGenerator* pIG);
	~XnVImageMessage();

	/**
	* Create a new Message of the same type (Image Message), with a clone of the same data (the same Image Generator)
	*
	* @return	The new message
	*/
	XnVMessage* Clone() const;

	/**
	* The default type for Image Messages - "Image"
	*/
	static const XnChar* ms_strType;
protected:
	const xn::ImageGenerator* m_pIM;
};
#endif
