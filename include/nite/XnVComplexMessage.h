/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_COMPLEX_MESSAGE_H_
#define _XNV_COMPLEX_MESSAGE_H_

#include "XnVMessage.h"

/**
* A XnVComplexMessage is a Message which holds additional Messages
*/
class XNV_NITE_API XnVComplexMessage :
	public XnVMessage
{
public:
	/**
	* Constructor. Create a new empty Complex Message
	*
	* @param	[in]	strType	The type of the the complex message. Default is "Complex"
	*/
	XnVComplexMessage(const XnChar* strType = ms_strType);
	virtual ~XnVComplexMessage();

	/**
	* Get the internal Message that matches the type
	*
	* @param	[in]	strType	The type of the internal message
	*
	* @return	The internal Message that matches the type, or NULL if one doesn't exist.
	*/
	XnVMessage* operator[](const XnChar* strType);

	/**
	* Add a Message to the Complex Message
	*
	* @param	[in]	pMessage	The Message to add to the Complex Message
	*/
	void Add(XnVMessage* pMessage);
	/**
	 * Remove a message from the Complex Message
	 *
	 * @param	[in]	strType		The type of the Message to remove
	 *
	 * @return	The Message matching the type, NULL if no such message
	 */
	XnVMessage* Remove(const XnChar* strType);
	/**
	* Remove all internal Messages
	*/
	void Clear();

	/**
	* Create a new Message of the same type (Complex Message), with the same data (the same internal Messages)
	*
	* @return	The new message
	*/
	XnVMessage* Clone() const;

	/**
	* The default type for Complex Messages - "Complex"
	*/
	static const XnChar* ms_strType;
private:
	XnVMessage** m_pMessages;
	XnUInt32 m_nNextAvailable;
	XnUInt32 m_nSize;
};

#endif // _XNV_COMPLEX_MESSAGE_H_
