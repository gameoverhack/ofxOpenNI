/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MESSAGE_MUX_H_
#define _XNV_MESSAGE_MUX_H_

#include <XnStringsHash.h>

#include "XnVFilter.h"
#include "XnVComplexMessage.h"

/**
* A XnVMessageMux is a Filter which created Complex Messages from multiple other Messages
*/
class XNV_NITE_API XnVMessageMux :
	public XnVFilter
{
public:
	/**
	* The Message Mux can run in several different modes
	*/
	enum XnVMuxMode
	{
		/** Send a Message anytime any Message is received */
		XNV_MUX_MODE_ANY,
		/** Send a Message only when all Messages are received */
		XNV_MUX_MODE_ALL,
		/** Send a Message when a specific type is received */
		XNV_MUX_MODE_SPECIFIC,
		/** Illegal */
		XNV_MUX_MODE_LAST
	};

	/**
	* Constructor. Default mode is XNV_MUX_MODE_ANY
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVMessageMux(const XnChar* strName = "XnVMessageMux");

	~XnVMessageMux();
	/**
	* Expect another type
	*
	* @param	[in]	strType	The type to be expected
	*/
	void AddType(const XnChar* strType);
	/**
	* Handle a Message. Add it to a Complex Message, and decide whether or not to send that Message
	*
	* @param	[in]	pMessage	The new Message
	*/
	void Update(XnVMessage* pMessage);

	/**
	* Change the mode
	*
	* @param	[in]	eMode	The new mode
	*/
	void SetMode(XnVMuxMode eMode);
	/**
	* Change the mode to a specific type
	* 
	* @param	[in]	strType	The type to use
	*/
	void SetMode(const XnChar* strType);
protected:
	void HandleSingleMessage(XnVMessage* pMessage);

	XnVComplexMessage* m_pComplexMessage;
	XnVMuxMode m_eMode;
	XnChar* m_strSpecific;

	XnStringsHash m_hTypes;
	XnUInt32 m_nExpected;
	XnList m_hCurrent;
};

#endif
