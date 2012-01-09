/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_IMAGE_GENERATOR_H_
#define _XNV_IMAGE_GENERATOR_H_

#include "XnVDeviceFilter.h"

/**
* A XnVImageGenerator is a Message Generator that generates Image Messages.
*/
class XNV_NITE_API XnVImageGenerator :
	public XnVContextFilter
{
public:
	/**
	* Constructor. Create a new Image Generator
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVImageGenerator(const XnChar* strName = "XnVImageGenerator");
	~XnVImageGenerator();

	/**
	* Generate an Image Message containing the Image Generator of the Context.
	*
	* @param	[in]	pContext	The Context from which the Image Generator will be taken.
	*/
	void Update(const xn::Context* pContext);
	/**
	* Generate an Image Message containing the given Image Generator.
	*
	* @param	[in]	pIG	The Image Generator to use in the Image Message
	*/
	void Update(const xn::ImageGenerator* pIG);
};

#endif
