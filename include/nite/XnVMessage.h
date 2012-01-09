/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MESSAGE_H_
#define _XNV_MESSAGE_H_

#include "XnVNiteDefs.h"

// Macros to get a specific part of the Message, if such a specific part exists
#define XNV_GET_SPECIFIC_MESSAGE_BY_TYPE(pMessage, SpecificType, strTypeDescription) \
	dynamic_cast<SpecificType*>((*pMessage)[strTypeDescription])

#define XNV_GET_SPECIFIC_MESSAGE(pMessage, SpecificType) \
	XNV_GET_SPECIFIC_MESSAGE_BY_TYPE(pMessage, SpecificType, SpecificType::ms_strType)

/**
* A XnVMessage is a general Message that holds some data. It will be sent between Generators and Listeners,
* thus passing data along the framework
*/
class XNV_NITE_API XnVMessage
{
public:
	/**
	* Constructor. Create a new Message, with a specific type and data
	*
	* @param	[in]	strType	The type of this Message
	* @param	[in]	pData	the data of this Message
	*/
	XnVMessage(const XnChar* strType, void* pData);
	virtual ~XnVMessage();

	/**
	* Get the inner data
	* 
	* @return The data in the Message
	*/
	void* GetData();

	/**
	* Get the Message of this type that in there.
	* Will generally return itself if the types match
	*
	* @param	[in]	strType	The type of the requested Message
	*
	* @return	A message of this type, or NULL if one doesn't exist
	*/
	virtual XnVMessage* operator[](const XnChar* strType);

	/**
	* Get the type of this message
	*
	* @return	The type of the Message
	*/
	const XnChar* GetType() const;
	/**
	* Abstract function. Any Message should know how to clone itself, along with its internal data
	*
	* @return	A new message with the same data
	*/
	virtual XnVMessage* Clone() const = 0;
protected:
	XnChar* m_strType;
	void* m_pData;

	XnBool m_bFreeOnDestruction;
};

#endif // _XNV_MESSAGE_H_
