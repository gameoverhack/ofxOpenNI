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
	void GenerateReplaced(XnVMessage* pMessage, XnVMultipleHands& newHands);

	/**
	 * Change the primary ID. If the new primary doesn't exist, it will be turned off.
	 * This takes affect inside the GenerateReplaced function.
	 *
	 * @param	[in]	nNewPrimary	The ID that is to be the new primary. Use 0 to return to default
	 */
	void OverridePrimary(XnUInt32 nNewPrimary);
};

#endif // _XNV_POINT_FILTER_H_
