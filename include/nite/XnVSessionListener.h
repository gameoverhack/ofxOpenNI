/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SESSION_LISTENER_H_
#define _XNV_SESSION_LISTENER_H_

#include "XnVNiteDefs.h"
#include "XnVNiteEvents.h"

/**
* A Session Listener will register to a Session Generator, which will call it when a Session event happens
*/
class XNV_NITE_API XnVSessionListener
{
public:
	/**
	 * Type for callbacks when a focus gesture has started to be recognized
	 */
	typedef void (XN_CALLBACK_TYPE *OnFocusStartDetectedCB)(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress, void* UserCxt);
	/**
	* Type for callbacks when a session has started
	*/
	typedef void (XN_CALLBACK_TYPE *OnSessionStartCB)(const XnPoint3D& ptPosition, void* UserCxt);
	/**
	* Type for callbacks when a session has ended
	*/
	typedef void (XN_CALLBACK_TYPE *OnSessionEndCB)(void* UserCxt);

	virtual void OnSessionStart(const XnPoint3D& ptPosition) = 0;
	virtual void OnSessionEnd() = 0;
	virtual void OnFocusStartDetected(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress) = 0;
};


#endif // _XNV_SESSION_LISTENER_H_
