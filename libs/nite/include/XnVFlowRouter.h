/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_FLOW_ROUTER_H_
#define _XNV_FLOW_ROUTER_H_

#include "XnVMessageListener.h"

class XnStringsHash;

/**
* A XnVFlowRouter is a Message Listener, that holds another single Listener internally,
* and sends any Message it gets to that Listener.
* The single internal Listener (referred to as the Active Control) may be changed,
* thus enabling to create the application's automaton.
*/
class XNV_NITE_API XnVFlowRouter :
	public XnVMessageListener
{
public:
	/**
	* Constructor. Create a new Flow Router
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVFlowRouter(const XnChar* strName = "XnVFlowRouter");
	virtual ~XnVFlowRouter();

	/**
	* Change the active Control.
	* If a session is kept, it is closed to the old Active Control (CloseOldSession),
	* and then opened to the new Active Control (OpenNewSession)
	*
	* @param	[in]	pActive	The new Active Control
	*/
	XnStatus SetActive(XnVMessageListener* pActive);
	/**
	* Get the current active Control
	*
	* @return	The current active Control
	*/
	XnVMessageListener* GetActive() const;

	/**
	* Send any Message received to the active Control
	*
	* @param	[in]	pMessage	The Message to route to the active Control
	*/
	void Update(XnVMessage* pMessage);

	/**
	* Clear the Multi-threading queue
	*/
	void ClearQueue();
protected:
	virtual void CloseOldSession();
	virtual void OpenNewSession();
	XnVMessageListener* m_pActive;

	XnStringsHash* m_pProperties; // for last points
};

#endif
