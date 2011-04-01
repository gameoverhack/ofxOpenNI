/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_FILTER_H_
#define _XNV_POINT_FILTER_H_

#include "XnVMessageGenerator.h"
#include "XnVPointControl.h"

/**
* A filter (listener/generator) which expects its messages to be PointMessages.
*/
class XNV_NITE_API XnVPointFilter :
	public XnVMessageGenerator,
	public XnVPointControl
{
public:
	/**
	* Creation.
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVPointFilter(const XnChar* strName = "XnVPointFilter");

	/**
	 * Send the receives message as-is, allowing Point based callbacks.
	 */
	void Update(XnVMessage* pMessage);

	/**
	* Clear the Multi-threading queue
	*/
	void ClearQueue();
	/**
	 * Replace the point part of the message, and generate it
	 *
	 * @param	[in]	pMessage	The message now handling (expects a PointMessage or a ComplexMessage with an internal PointMessage)
	 * @param	[in]	newHands	The hands that are to replace the ones in the original message
	 */
	void GenerateReplaced(XnVMessage* pMessage, const XnVMultipleHands& newHands);
};

#endif // _XNV_POINT_FILTER_H_
