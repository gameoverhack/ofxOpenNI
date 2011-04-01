/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_VIRTUAL_COORDINATES_H_
#define _XNV_VIRTUAL_COORDINATES_H_

#include "XnVPointFilter.h"

class XnVVirtualCoordinatesInternal;

#define XNV_VC_MAX_FILE_NAME	2048

/**
 * The virtual coordinates filter learns the user's virtual plane,
 * and translates all points to coordinates relative to that plane
 */
class XNV_NITE_API XnVVirtualCoordinates :
	public XnVPointFilter
{
public:
	/**
	 * Constructor: Create a new Virtual Coordinates Filter
	 *
	 * @param	[in]	strINI	Initialization file
	 * @param	[in]	strName	Name of the control, for log purposes.
	 */
	XnVVirtualCoordinates(const XnChar* strINI, const XnChar* strName = "XnVVirtualCoordintates");
	virtual ~XnVVirtualCoordinates();

	void Update(XnVMessage* pMessage);
	/**
	 * Translate a new batch of points
	 *
	 * @param	[in]	hands	The current points
	 */
	void Update(const XnVMultipleHands& hands);

	/**
	* Called when a point is created.
	*
	* @param	[in]	pContext	The hand context of the newly created point
	*/
	void OnPointCreate(const XnVHandPointContext* pContext);
	/**
	* Called when a point is updated.
	*
	* @param	[in]	pContext	The hand context of the point
	*/
	void OnPointUpdate(const XnVHandPointContext* pContext);
	/**
	* Called when a point is destroyed.
	*
	* @param	[in]	nID	the ID of the destroyed point
	*/
	void OnPointDestroy(XnUInt32 nID);
protected:
	XN_DECLARE_DEFAULT_HASH_DECL(XNV_NITE_API, XnUInt32, XnVVirtualCoordinatesInternal*, XnVIntVCIHash);

	void Clear();
	virtual void UpdateVirtualPoint(XnVVirtualCoordinatesInternal* pVC, const XnVHandPointContext* pContext, XnVHandPointContext* pLocalContext);
	XnVVirtualCoordinatesInternal* GetLocalContext(XnUInt32 nID);

	XnVIntVCIHash m_VCContexts;

	XnChar m_strINI[XNV_VC_MAX_FILE_NAME];

	XnVMultipleHands m_Hands;
};



#endif // _XNV_VIRTUAL_COORDINATES_H_