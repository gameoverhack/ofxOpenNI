/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_MESSAGE_H_
#define _XNV_POINT_MESSAGE_H_

#include "XnVMessage.h"
#include "XnVMultipleHands.h"

/**
* A XnVPointMessage is a Message holding a Multi Hands
*/
class XNV_NITE_API XnVPointMessage :
	public XnVMessage
{
public:
	/**
	* Constructor. Create a Point Message with a Multi Hands.
	*
	* @param	[in]	pHands	The Multi Hands to use in the Point Message
	*/
	XnVPointMessage(const XnVMultipleHands* pHands);
	~XnVPointMessage();

	/**
	* Create a new Message of the same type (Point Message), with a clone of the same data (the same Multi Hands)
	*
	* @return	The new message
	*/
	XnVMessage* Clone() const;

	/**
	* The default type for Point Messages - "Point"
	*/
	static const XnChar* ms_strType;
protected:
	const XnVMultipleHands* m_pHands;
};

#endif
