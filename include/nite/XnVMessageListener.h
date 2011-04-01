/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MESSAGE_LISTENER_H_
#define _XNV_MESSAGE_LISTENER_H_

#include <XnThreadSafeQueue.h>
#include "XnVNiteEvents.h"
#include "XnVMessage.h"

/**
* A XnVMessageListener is a general listener, that received Messages and handles them.
* It supports Multi-Threaded work, by knowing the thread it works in,
* and putting Messages in a Queue when accessed from other threads.
*/
class XNV_NITE_API XnVMessageListener
{
public:
	/**
	* Type for callbacks after a Message is handled
	*/
	typedef void (XN_CALLBACK_TYPE* MessageUpdateCB)(XnVMessage* pMessage, void* cxt);
	/**
	* Type for callbacks on activation
	*/
	typedef void (XN_CALLBACK_TYPE* ActivateCB)(void* cxt);
	/**
	* Type for callbacks on deactivation
	*/
	typedef void (XN_CALLBACK_TYPE* DeactivateCB)(void* cxt);

	/**
	* Constructor. Create a new Message Listener
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVMessageListener(const XnChar* strName = "XnVMessageListener");
	virtual ~XnVMessageListener();

	/**
	* This method handles multi-thread support, calling Update only when run from the proper thread
	* and copying the Message to a Queue when not.
	*
	* @param	[in]	pMessage	The Message that should be handled
	*/
	void BaseUpdate(XnVMessage* pMessage);
	/**
	* This method will be implemented by descendants, handling the Message
	*
	* @param	[in]	pMessage	The Message that was received
	*/
	virtual void Update(XnVMessage* pMessage) = 0;
	
	/**
	* Handle Control Messages. By default, it calls Activate for Activation Messages.
	* Can be reimplemented to handle additional Message types.
	* Is called from BaseUpdate, before Update.
	*
	* @param	[in]	pMessage	The message to handle
	*/
 	virtual void HandleCCMessages(XnVMessage* pMessage);

	/**
	* This method is called (by the default HandleCCMessages) when an Activation Message is received.
	*
	* @param	[in]	bActive	TRUE if this listener is now active, FALSE if it is now inactive
	*/
	virtual void Activate(XnBool bActive) {}

	/**
	* Get the listener's name. Mostly for Log use.
	*
	* @return The current Listener's name
	*/
	const XnChar* GetListenerName() const;

	/**
	* Supply the Message Listener with run-time from its thread. Read from the Queue, and BaseUpdate.
	*
	* @param	[in]	nSessionMaxLength	Maximum number of Messages to be handled in a single Run
	*/
	void Run(XnUInt32 nSessionMaxLength = ms_nSessionDefaultLength);
	/**
	* Open a thread, and run the Listener in it.
	*
	* @return	XN_STATUS_OK if OK, other on error
	*/
	XnStatus RunAsThread();
	/**
	* Change the working thread, in which actual work is done
	*
	* @param	[in]	hThread	The ID of the relevant thread
	*/
	void SetThreadId(XN_THREAD_ID hThread);
	/**
	* Change the working thread to the one from which it is called
	*/
	void SetCurrentThread();

	/**
	* Register a callback to be called after the Message is handled
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterUpdate(void* cxt, MessageUpdateCB CB);
	/**
	* Register for the activation event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterActivate(void* cxt, ActivateCB CB);
	/**
	* Register for the deactivation event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterDeactivate(void* cxt, DeactivateCB CB);

	/**
	* Unregister from the Message Update
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterUpdate(XnCallbackHandle hCB);
	/**
	* Unregister from the activation event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterActivate(XnCallbackHandle hCB);
	/**
	* Unregister from the deactivation event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterDeactivate(XnCallbackHandle hCB);

	/**
	* Change the multi-thread mode.
	*
	* @param	[in]	bMode	The new mode
	*/
	void SetThreadProtectionQueueMode(XnBool bMode);
	/**
	* Get the current multi-thread mode
	*
	* @return	TRUE if queues are used, FALSE if Update will be called from anywhere
	*/
	XnBool GetThreadProtectecionQueueMode() const;
	/**
	 * Clear the Multi-thread queue
	 */
	virtual void ClearQueue();
protected:
	XN_DECLARE_THREAD_SAFE_QUEUE_DECL(XNV_NITE_API, XnVMessage*, XnVMessageQueue);
	XN_DECLARE_EVENT_1ARG(XnVMessageSpecificEvent, XnVMessageEvent, XnVMessage*, pMessage);

	XnBool IsInActivityThread() const;

	static XN_THREAD_PROC MessageListenerThread(XN_THREAD_PARAM param);
	void MainLoop();

	XN_THREAD_ID m_hActivityThreadId;

	XN_THREAD_HANDLE m_hActivityThread; // When thread is internal
	XnBool m_bInternalThreadAlive;
	XnBool m_bInternalThreadKill;

	XnBool m_bThreadProtectionQueueMode;

	static const XnUInt32 ms_nSessionDefaultLength;

	XnVMessageQueue m_MessageQueue;

	XnVMessageSpecificEvent m_UpdateCBs;
	XnVEvent m_ActivateCBs;
	XnVEvent m_DeactivateCBs;

	XnChar* m_strListenerName;

	XN_CRITICAL_SECTION_HANDLE m_hListenerCS;
};

#endif
