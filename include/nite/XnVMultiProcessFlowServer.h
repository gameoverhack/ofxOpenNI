/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_SERVER_H_
#define _XNV_POINT_SERVER_H_

#include "XnVPointControl.h"
#include "XnVSessionListener.h"

struct XnVNiteMultiprocessData;
class XnVMultiprocessWriteSynchronizer;

/**
* XnVMultiProcessFlowServer is both a XnVPointControl and a SessionListener. When placed into a NITE flow
* it stores the latest state it has according to events that arrive (PointCreate/Update/Destroy,
* SessionCreate/SessionDestroy, etc.) and calls WriteState.
* When WriteState() is called, it publishes the current state into a shared memory section.
* Clients (XnVMultiProcessFlowClient objects) which read the published state can then trigger the same
* events in different processes.
* NOTE: The object is NOT thread-safe, meaning, the WriteState and Reset methods must be called
*		 from the same thread that XnVPointListener::Update() is called on the point-listener
*		 which is connected to the server
*/
class XNV_NITE_API XnVMultiProcessFlowServer :
	public XnVPointControl,
	public XnVSessionListener
{
public:
	/**
	* Create a XnVMultiProcessFlowServer
	*
	* @param	[in]	strSectionName	The name of the Shared Memory section to which we will write
	* @param	[in]	strName				Name of the control, for log purposes.
	*/
	XnVMultiProcessFlowServer(const XnChar* strSectionName, const XnChar* strName = "XnVMultiProcessFlowServer");
	~XnVMultiProcessFlowServer();

	/**
	* Initialize the shared-memory
	* Must be called before the first call to WriteState.
	*
	* @return	XN_STATUS_OK on successful init, XN_STATUS_* error code otherwise
	*/
	XnStatus Initialize();

	/**
	* Reset stored state. Does not publish the state to clients. Calling WriteState()
	* directly after calling this method will destroy the session and all points in clients.
	* NOTE: Reset MUST be called before re-connecting a Server into a Nite flow, or undefined
	* behavior will occur due to stale state data inside the server
	*/
	void Reset();

	/**
	 * Handle new hands batch. Write to internal state, to be written to the shared memory.
	 *
	 * @param	[in]	hands	The current state of the hands
	 */
	void Update(const XnVMultipleHands& hands);
protected:

	XnStatus WriteState();

	void OnPointCreate(const XnVHandPointContext* pContext);
	void OnPointUpdate(const XnVHandPointContext* pContext);
	void OnPointDestroy(XnUInt32 nID);
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptSessionStarter);
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);
	void OnPrimaryPointDestroy(XnUInt32 nID);

	void OnSessionStart(const XnPoint3D& ptPosition);
	void OnSessionEnd();
	void OnFocusStartDetected(const XnChar* strFocus, const XnPoint3D& ptPosition, XnFloat fProgress);
private:
	void CheckSessionStarted();

	XnUInt32 m_nWriteCount;
	XnVMultiprocessWriteSynchronizer* m_pWritingLock;
	XnVNiteMultiprocessData* m_pCurrentState;
	XnVIntHash m_IdToIndex;
	XnBool m_bFocusStartSinceUpdate;

	const XnVMultipleHands* m_pCurrentHands;
};

#endif // _XNV_POINT_SERVER_H_
