/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MULTI_PROCESS_FLOW_CLIENT_H_
#define _XNV_MULTI_PROCESS_FLOW_CLIENT_H_

#include "XnVSessionGenerator.h"
#include "XnVMultipleHands.h"

struct XnVNiteMultiprocessData;
class XnVMultiprocessReadSynchronizer;
class IdToContextMapping;

/**
 * The multi process flow client is a near-equivalent to an XnVSessionManager, but which receives
 * its data from a shared-memory buffer, instead of understanding a depth-map directly.
 * The XnVMultiProcessFlowClient is intended to be used to read the data written by an
 * XnVMultiProcessFlowServer object.
 * Multiple readers are allowed for a single writer.
 * NOTE: Since communication is strictly one-way (Server to Client(s)), The LosePoint()
 *       and Reset() methods of XnVSessionGenerator do nothing. If you wish to implement
 *		 such functionality in your client process, it must be done using a separate
 *		 communication channel
 */
class XNV_NITE_API XnVMultiProcessFlowClient :
	public XnVSessionGenerator
{
public:
	/**
	 * Create a XnVMultiProcessFlowClient
	 *
	 * @param	[in]	sOutputSectionName	The name of the Shared Memory section which we will read from
	 * @param	[in]	strName				Name of the control, for log purposes.
	 *
	 */
	XnVMultiProcessFlowClient(const XnChar* sOutputSectionName, const XnChar* strName = "XnVMultiProcessFlowClient");
	/**
	 * Cleanup
	 */
	~XnVMultiProcessFlowClient();

	/**
	 * Initialize the XnVMultiProcessFlowClient
	 * Must be called before the first call to ReadState.
	 *
	 * @return	XN_STATUS_OK on successful init, XN_STATUS_* error code otherwise
	 */
	XnStatus Initialize();

	/**
	 * Read a new state from the shared section, and trigger events according to the difference
	 * between the new state and the old one.
	 * This function is blocking.
	 *
	 * @return	XN_STATUS_OK on successful init, XN_STATUS_* error code otherwise
	 */
	XnStatus ReadState();

	/**
	 * Lose point - required API of XnVSessionGenerator. Can't lose points in multi-process.
	 *
	 * @return XN_STATUS_ERROR
	 */
	XnStatus LosePoint(XnUInt32 nID);
	/**
	* Lose all points - required API of XnVSessionGenerator. Can't lose points in multi-process.
	*
	* @return XN_STATUS_ERROR
	*/
	XnStatus LosePoints();

	XnStatus EndSession();
	XnStatus ForceSession(const XnPoint3D& ptFocus);
	XnStatus TrackPoint(const XnPoint3D& pt);
protected:
	XN_DECLARE_DEFAULT_HASH_DECL(XNV_NITE_API, XnUInt32, XnVHandPointContext*, XnVIntContextHash);

	XnVHandPointContext* GetLocalContext(XnUInt32 nID);

	void SendEventsForStateDiff(const XnVNiteMultiprocessData& NewState);

	XnVMultiprocessReadSynchronizer* m_pReadingLock;
	XnVNiteMultiprocessData* m_pLastReadState;
	XnVHandPointContext* m_pInputHandContext;
	XnPoint3D m_ptInputHandPoint;
	XnVIntContextHash* m_pExistingPointContexts;

	XnVMultipleHands m_Hands;
}; // XnVMultiProcessFlowClient

#endif // _XNV_MULTI_PROCESS_FLOW_CLIENT_H_
