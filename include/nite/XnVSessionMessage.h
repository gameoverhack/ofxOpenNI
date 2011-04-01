/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SESSION_MESSAGE_H_
#define _XNV_SESSION_MESSAGE_H_

#include "XnVCCMessage.h"

/**
* A XnVSessionMessage is a Message holding a boolean value, indicating session start/stop
*/
class XNV_NITE_API XnVSessionMessage :
	public XnVCCMessage
{
public:
	/**
	* Constructor. Create a Session Message with a boolean. Focus point is (0,0,0)
	*
	* @param	[in]	bStart	Indication of session start/stop
	*/
	XnVSessionMessage(XnBool bStart);
	/**
	* Constructor. Create a Session Message with a boolean, and a focus point.
	*
	* @param	[in]	bStart	Indication of session start/stop
	* @param	[in]	ptFocus	The focus point
	*/
	XnVSessionMessage(XnBool bStart, const XnPoint3D& ptFocus);
	~XnVSessionMessage();

	XnVMessage* Clone() const;

	/**
	* The default type for Session Messages - "Session"
	*/
	static const XnChar* ms_strType;

	const XnPoint3D& GetFocusPoint();
protected:
	XnPoint3D m_ptFocus;
};

#endif // _XNV_SESSION_MESSAGE_H_
