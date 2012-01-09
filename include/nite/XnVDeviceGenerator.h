/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEVICE_GENERATOR_H_
#define _XNV_DEVICE_GENERATOR_H_

#include <XnCppWrapper.h>

#include "XnVMessageGenerator.h"

/**
* A XnVContextGenerator is a Message Generator that generates Context Messages.
*/
class XNV_NITE_API XnVContextGenerator :
	public XnVMessageGenerator
{
public:
	/**
	* Constructor. Create a new Context Generator
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVContextGenerator(const XnChar* strName = "XnVContextGenerator");
	~XnVContextGenerator();

	/**
	* Generate a Context Message containing the specific Context
	*
	* @param	[in]	pContext	The context to encapsulate in a Message
	*/
	void Update(const xn::Context* pContext);
};

#endif // _XNV_DEVICE_GENERATOR_H_
