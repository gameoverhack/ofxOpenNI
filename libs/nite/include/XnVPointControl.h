/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_CONTROL_H_
#define _XNV_POINT_CONTROL_H_

#include "XnVMessageListener.h"
#include "XnVMultipleHands.h"

class XnEvent;
class XnVHandPointContextSpecificEvent;
class XnVMultipleHandsSpecificEvent;
class XnVHandPointContextPointSpecificEvent;
class XnVUintHandPointContextSpecificEvent;
class XnVUintSpecificEvent;

/**
* A XnVPointControl is a Message Listener that expects Point Messages.
* It is meant to be a base class for Controls that are Point-based.
*/
class XNV_NITE_API XnVPointControl :
	public XnVMessageListener
{
public:
	/**
	* Type for Point Create callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PointCreateCB)(const XnVHandPointContext* pContext, void* cxt);
	/**
	* Type for Point Update callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PointUpdateCB)(const XnVHandPointContext* pContext, void* cxt);
	/**
	* Type for Point Destroy callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PointDestroyCB)(XnUInt32 nID, void* cxt);

	/**
	* Type for Primary Point Create callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PrimaryPointCreateCB)(const XnVHandPointContext* pContext, const XnPoint3D& ptFocus, void* cxt);
	/**
	* Type for Primary Point Update callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PrimaryPointUpdateCB)(const XnVHandPointContext* pContext, void* cxt);
	/**
	* Type for Primary Point Replace callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PrimaryPointReplaceCB)(XnUInt32 nOldId, const XnVHandPointContext* pContext, void* cxt);
	/**
	* Type for Primary Point Destroy callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*PrimaryPointDestroyCB)(XnUInt32 nID, void* cxt);

	typedef void (XN_CALLBACK_TYPE*NoPointsCB)(void* cxt);

	/**
	* Type for Multiple Hands callbacks
	*/
	typedef void (XN_CALLBACK_TYPE*HandsUpdateCB)(const XnVMultipleHands& mh, void* cxt);

	/**
	* Constructor. Create a new Point Control
	*
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVPointControl(const XnChar* strName = "XnVPointControl");
	~XnVPointControl();
	/**
	* Handle a Message. This method extracts the inner Multi Hands, and calls the other Update method.
	*/
	void Update(XnVMessage* pMessage);
	/**
	* Handle a Multi Hands. This is the method users will implement.
	*/
	virtual void Update(const XnVMultipleHands& hands);

	/**
	* Handle a new Point Create event
	*
	* @param	[in]	pContext	The context of the new Point
	*/
	virtual void OnPointCreate(const XnVHandPointContext* pContext) {}
	/**
	* Handle an existing Point Update event
	*
	* @param	[in]	pContext	The context of the Point
	*/
	virtual void OnPointUpdate(const XnVHandPointContext* pContext) {}
	/**
	* Handle a existing Point Destroy event
	*
	* @param	[in]	nID	The ID of the destroyed Point
	*/
	virtual void OnPointDestroy(XnUInt32 nID) {}

	/**
	 * Handle the primary point create
	 *
	 * @param	[in]	pContext			The context of the primary point
	 * @param	[in]	ptSessionStarter	The point in which the session started
	 */
	virtual void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptSessionStarter) {}
	/**
	 * Handle an update of the Primary Point
	 *
	 * @param	[in]	pContext	The context of the primary point
	 */
	virtual void OnPrimaryPointUpdate(const XnVHandPointContext* pContext) {}
	/**
	 * Handle a change of the Primary Point
	 *
	 * @param	[in]	nOldId		The previous primary point
	 * @param	[in]	pContext	The context of the new primary point
	 */
	virtual void OnPrimaryPointReplace(XnUInt32 nOldId, const XnVHandPointContext* pContext) {}
	/**
	 * Handle the primary point destruction
	 * 
	 * @param	[in]	nID		The ID of the last primary point
	 */
	virtual void OnPrimaryPointDestroy(XnUInt32 nID) {}

	/**
	 * There are no more points
	 */
	virtual void OnNoPoints() {}

	/**
	* Register a callback for when a point is created
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPointCreate(void* cxt, PointCreateCB CB);
	/**
	* Register a callback for when a point is updated
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPointUpdate(void* cxt, PointUpdateCB CB);
	/**
	* Register a callback for when a point is destroyed
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPointDestroy(void* cxt, PointDestroyCB CB);

	/**
	* Register a callback for when the Primary Point is created
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPrimaryPointCreate(void* cxt, PrimaryPointCreateCB CB);
	/**
	* Register a callback for when the Primary Point is updated
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPrimaryPointUpdate(void* cxt, PrimaryPointUpdateCB CB);
	/**
	* Register a callback for when the Primary Point is replaced
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPrimaryPointReplace(void* cxt, PrimaryPointReplaceCB CB);
	/**
	* Register a callback for when the Primary Point is destroyed
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPrimaryPointDestroy(void* cxt, PrimaryPointDestroyCB CB);
	/**
	* Register a callback for when there are no longer any points
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterNoPoints(void* cxt, NoPointsCB CB);

	/**
	* Register a callback for when multiple hands are available
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterHandsUpdate(void* cxt, HandsUpdateCB CB);

	/**
	* Unregister a callback for when a point is created
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPointCreate(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when a point is updated
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPointUpdate(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when a point is destroyed
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPointDestroy(XnCallbackHandle hCB);

	/**
	* Unregister a callback for when the Primary Point is created
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPrimaryPointCreate(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when the Primary Point is updated
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPrimaryPointUpdate(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when the Primary Point is replaced
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPrimaryPointReplace(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when the Primary Point is destroyed
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPrimaryPointDestroy(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when a point is destroyed
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterNoPoints(XnCallbackHandle hCB);

	/**
	* Unregister a callback for when multiple hands are available
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterHandsUpdate(XnCallbackHandle hCB);

	/**
	* Get the current primary point ID
	* 
	* @return	The Primary Point's ID
	*/
	XnUInt32 GetPrimaryID() const;
private:
	XnVHandPointContextSpecificEvent* m_pPointCreateCBs;
	XnVHandPointContextSpecificEvent* m_pPointUpdateCBs;
	XnVUintSpecificEvent* m_pPointDestroyCBs;

	XnVHandPointContextPointSpecificEvent* m_pPrimaryPointCreateCBs;
	XnVHandPointContextSpecificEvent* m_pPrimaryPointUpdateCBs;
	XnVUintHandPointContextSpecificEvent* m_pPrimaryPointReplaceCBs;
	XnVUintSpecificEvent* m_pPrimaryPointDestroyCBs;

	XnVEvent* m_pNoPointsCBs;

	XnVMultipleHandsSpecificEvent* m_pHandsUpdateCBs;

	XnUInt32 m_nPrimaryID;
protected:
	XnUInt32 m_nOverridePrimary;
};

#endif
