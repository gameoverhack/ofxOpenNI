/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_DEVICE_FILTER_H_
#define _XNV_DEVICE_FILTER_H_

#include "XnVMessageGenerator.h"
#include "XnVDeviceControl.h"

/**
 * A filter (listener/generator) which expects its messages to be ContextMessages.
 */
class XNV_NITE_API XnVContextFilter :
	public XnVMessageGenerator,
	public XnVContextControl
{
public:
	/**
	 * Creation.
	 *
	 * @param	[in]	strName		Name of the control, for log purposes.
	 */
	XnVContextFilter(const XnChar* strName = "XnVContextFilter");

	/**
	 * Clear the Multi-threading queue
	 */
	void ClearQueue();
};

#endif // _XNV_DEVICE_FILTER_H_
