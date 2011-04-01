/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_AREA_H_
#define _XNV_POINT_AREA_H_

#include "XnVPointFilter.h"
#include "XnVSessionGenerator.h"
#include "XnVPointTracker.h"

/**
* Defines a legal bounding box. Any point that goes outside it will seem to be lost to anyone further along the chain - 
* it will be silenced by the PointArea.
* If a point is silent for more than a threshold, it will be removed from tracking (for real).
*/
class XNV_NITE_API XnVPointArea :
	public XnVPointFilter
{
public:
	/**
	* Type for callbacks to be called when a point is silenced
	*/
	typedef void (XN_CALLBACK_TYPE*PointSilencedCB)(XnUInt32 nID, void* cxt);
	/**
	* Type for callbacks to be called when a previously silenced point is revived
	*/
	typedef void (XN_CALLBACK_TYPE*PointRevivedCB)(XnUInt32 nID, void* cxt);
	/**
	* Type for callbacks to be called when a previously silenced point is really removed
	*/
	typedef void (XN_CALLBACK_TYPE*SilentPointRemovedCB)(XnUInt32 nID, void* cxt);

	/**
	* Constructor. Create a new Point Area from a bounding box
	*
	* @param	[in]	bbArea		The area in which points are legal
	* @param	[in]	bRelative	TRUE if area is relative to focus point, FALSE if constant area
	* @param	[in]	strName		Name of the control, for log purposes.
	*/
	XnVPointArea(const XnBoundingBox3D& bbArea, XnBool bRelative = false, const XnChar* strName = "XnVPointArea");
	/**
	* Constructor. Create from two points, defining an axis-aligned 3d box.
	*
	* @param	[in]	ptMins	One of the points that define an axis-aligned 3D box
	* @param	[in]	ptMaxs	One of the points that define an axis-aligned 3D box
	* @param	[in]	bRelative	TRUE if area is relative to focus point, FALSE if constant area
	* @param	[in]	strName	The name of this Point Area, for Log use
	*/
	XnVPointArea(const XnPoint3D& ptMins, const XnPoint3D& ptMaxs, XnBool bRelative = false, const XnChar* strName = "XnVPointArea");

	/**
	* Change the area in which points are allowed to exist
	*
	* @param	[in]	bbArea	The 3D Real World coordinates
	* @param	[in]	bRelative	TRUE if area is relative to focus point, FALSE if constant area
	*/
	void ChangeArea(const XnBoundingBox3D& bbArea, XnBool bRelative = false);
	/**
	* Change the area in which points are allowed to exist
	*
	* @param	[in]	ptMins	One of the points that define an axis-aligned 3D box
	* @param	[in]	ptMaxs	One of the points that define an axis-aligned 3D box
	* @param	[in]	bRelative	TRUE if area is relative to focus point, FALSE if constant area
	*/
	void ChangeArea(const XnPoint3D& ptMins, const XnPoint3D& ptMaxs, XnBool bRelative = false);

	/**
	 * Handle a new Message (expected to be XnVPointMessage)
	 *
	 * @param	[in]	pMessage	The message to handle
	 */
	void Update(XnVMessage* pMessage);
	/**
	* Intersect points with internal area
	*
	* @param	[in]	hands	The existing hands
	*/
	void Update(const XnVMultipleHands& hands);

	/**
	* Register a callback for when a point is silenced
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPointSilenced(void* cxt, PointSilencedCB CB);
	/**
	* Register a callback for when a point is revived
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterPointRevived(void* cxt, PointRevivedCB CB);
	/**
	* Register a callback for when silent point is really removed
	*
	* @param	[in]	cxt	User's context. The callback will be called with that context.
	* @param	[in]	CB	The Callback
	*
	* @return	A handle to the callback, to allow unregistering.
	*/
	XnCallbackHandle RegisterSilentPointRemoved(void* cxt, SilentPointRemovedCB CB);

	/**
	* Unregister a callback for when a point is silenced
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPointSilenced(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when a point is revived
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterPointRevived(XnCallbackHandle hCB);
	/**
	* Unregister a callback for when silent point is really removed
	*
	* @param	[in]	hCB	The handle received when registering the callback.
	*/
	void UnregisterSilentPointRemoved(XnCallbackHandle hCB);

	/**
	 * Remove silent points after some delay from a SessionGenerator
	 *
	 * @param	[in]	nDelay	The delay in milliseconds between silencing a point and removing it
	 * @param	[in]	pGenerator	The SessionGenerator through which to remove the point
	 */
	void RemoveSilent(XnUInt64 nDelay, XnVSessionGenerator* pGenerator);
	/**
	 * Remove silent points after some delay from a PointTracker
	 *
	 * @param	[in]	nDelay	The delay in milliseconds between silencing a point and removing it
	 * @param	[in]	pTracker	The PointTracker through which to remove the point
	 */
	void RemoveSilent(XnUInt64 nDelay, XnVPointTracker* pTracker);

protected:
	XnBool InBoundingBox(const XnPoint3D& pt) const;

	XnBool IsSilent(XnUInt32 nID) const;

	void SilencePoint(XnUInt32 nID, XnFloat fTime);
	void RevivePoint(XnUInt32 nID);
	XnStatus CheckDelay(XnUInt32 nID, XnFloat fTime);

	void ChangeConstantArea(const XnBoundingBox3D& bbArea);
	void ChangeRelativeArea(const XnBoundingBox3D& bbArea);

	XnVMultipleHands m_FilteredHands;
	XnBoundingBox3D m_bbArea;

	XnBool m_bRelative;
	XnBoundingBox3D m_bbRelativeArea;

	XnVIntHash m_SilentPoints;

	XnVUintSpecificEvent m_PointSilencedCBs, m_PointRevivedCBs, m_SilentPointRemovedCBs;

	XnBool m_bRemoveByGenerator;
	XnBool m_bRemoveByTracker;
	XnVPointTracker* m_pRemovingTracker;
	XnVSessionGenerator* m_pRemovingGenerator;
	XnUInt64 m_nRemovalDelay;
};


#endif // _XNV_POINT_AREA_H_
