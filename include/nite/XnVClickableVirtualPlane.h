/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_CLICKABLE_VIRTUAL_PLANE_H_
#define _XNV_CLICKABLE_VIRTUAL_PLANE_H_

#include "XnVVirtualCoordinates.h"

class XnVClickableVirtualPlane;
class XnVIntPointHash;
class XnVIntIDHash;
class XnVUintPointSpecificEvent;

struct XnVClickableID
{
	XnVClickableID(const XnVHandPointContext* cxt, XnVClickableVirtualPlane* pPlane) :
pCxt(cxt), pCVP(pPlane)
{}

	const XnVHandPointContext* pCxt;
	XnVClickableVirtualPlane* pCVP;
};

/**
* This is a specific virtual coordinates, which can freeze points to allow a comfortable click.
*/
class XNV_NITE_API XnVClickableVirtualPlane :
	public XnVVirtualCoordinates
{
public:
	/**
	* Type for recognizing a click relative to the virtual plane.
	*/
	typedef void (XN_CALLBACK_TYPE* ClickCB)(XnUInt32 nID, const XnPoint3D& ptPosition, void* cxt);

	/**
	* Constructor. Create an instance, using the configuration file supplied as parameter.
	*
	* @param	[in]	strINI		Configuration INI file
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVClickableVirtualPlane(const XnChar* strINI, const XnChar* strName = "XnVClickableVirtualPlane");
	~XnVClickableVirtualPlane();

	/**
	* Register for the click event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterClick(void* cxt, ClickCB CB);
	/**
	* Unregister from the click event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterClick(XnCallbackHandle hCB);

	/**
	* Invoke the click event
	*
	* @param	[in]	ptPosition	The position of the click
	*/
	void Click(XnUInt32, const XnPoint3D& ptPosition);

	/**
	* Called when a point is created.
	*
	* @param	[in]	pContext	The hand context of the newly created point
	*/
	void OnPointCreate(const XnVHandPointContext* pContext);
	/**
	* Called when a point is destroyed.
	*
	* @param	[in]	nID	the ID of the destroyed point
	*/
	void OnPointDestroy(XnUInt32 nID);

	/**
	* Freeze the coordinates - a click is starting.
	*
	* @param	[in]	nID			The ID of the point to freeze
	* @param	[in]	ptPosition	The coordinates to send from now on
	*/
	void FreezeCoordinates(XnUInt32 nID, const XnPoint3D& ptPosition);
	/**
	* Unfreeze the coordinates - a click is no longer searched.
	*
	* @param	[in]	nID	The ID of the point to unfreeze.
	*/
	void UnfreezeCoordinates(XnUInt32 nID);
	/**
	 * Check if a specific point is frozen
	 *
	 * @param	[in]	nID	The ID the point
	 *
	 * @return	TRUE if the point is frozen, FALSE o/w
	 */
	XnBool IsFrozen(XnUInt32 nID) const;
protected:
	void UpdateVirtualPoint(XnVVirtualCoordinatesInternal* pVC, const XnVHandPointContext* pContext, XnVHandPointContext* pLocalContext);

	XnVIntPointHash* m_pFrozenPoints;
	XnVIntIDHash* m_pIDContexts;

	XnVUintPointSpecificEvent* m_pClickCBs;
};

#endif // _XNV_CLICKABLE_VIRTUAL_PLANE_H_
