/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEPTH_GENERATOR_H_
#define _XNV_DEPTH_GENERATOR_H_

#include <XnCppWrapper.h>

#include "XnVDeviceFilter.h"

/**
* A XnVDepthGenerator is a Message Generator that generates Depth Messages.
*/
class XNV_NITE_API XnVDepthGenerator :
	public XnVContextFilter
{
public:
	/**
	* Constructor. Create a new Depth Generator
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVDepthGenerator(const XnChar* strName = "XnVDepthGenerator");
	virtual ~XnVDepthGenerator();

	/**
	* Generate a Depth Message containing the Depth from the context.
	*
	* @param	[in]	pContext	The context from which the Depth Map will be taken.
	*/
	void Update(const xn::Context* pContext);
	/**
	* Generate a Depth Message containing the given Depth Generator.
	*
	* @param	[in]	pDG	The Depth Generator to use in the Depth Message
	*/
	void Update(const xn::DepthGenerator* pDG);
};

#endif // _XNV_MESSAGE_GENERATOR_H_
