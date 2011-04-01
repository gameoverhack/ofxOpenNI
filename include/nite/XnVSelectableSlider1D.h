/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SELECTABLE_SLIDER_1D_H_
#define _XNV_SELECTABLE_SLIDER_1D_H_

#include "XnVPointControl.h"
#include "XnVMultiItemHysteresis1D.h"
#include "XnVSlider1D.h"

class XnVBorderScrollController1D;

/**
* A Selectable slider is a 1D slider (along any of the x, y or axes) with a number of cells and borders,
* where the items can be selected by moving off axis.
*
* Item 0 is the leftmost when using AXIS_X, bottommost when using AXIS_Y and closest to the sensor when using AXIS_Z.
*/
class XNV_NITE_API XnVSelectableSlider1D :
	public XnVPointControl
{
public:
	/**
	* Type for hover event callback. Receives the index of the hovered item
	*/
	typedef void (XN_CALLBACK_TYPE *ItemHoverCB)(XnInt32 nItemIndex, void* pUserCxt);
	/**
	* Type for select event callback. Receives the index of the selected item and the direction of the selection.
	*/
	typedef void (XN_CALLBACK_TYPE *ItemSelectCB)(XnInt32 nItemIndex, XnVDirection nDirection, void* pUserCxt);
	/**
	* Type for scroll event callback.
	* Receives the number between -1 and 1, indicating where in the border the point is
	*/
	typedef void (XN_CALLBACK_TYPE *ScrollCB)(XnFloat fScrollValue, void* pUserCxt);
	/**
	* Type for value change event callback.
	* Receives a number between 0 and 1, indicating where in the slider the point is
	*/
	typedef void (XN_CALLBACK_TYPE *ValueChangeCB)(XnFloat fValue, void* pUserCxt);
	/**
	* Type for off axis event callback. Receives the direction of the off axis movement
	*/
	typedef void (XN_CALLBACK_TYPE *OffAxisMovementCB)(XnVDirection dir, void* pUserCxt);

	/**
	* Creation. Gets the number of items in the slider, the width of the border (on either side),
	* its axis, and more.
	*
	* @param	[in]	nCount				Number of items in the slider
	* @param	[in]	fBorderWidth		Size reserved from the slider for scrolling
	* @param	[in]	eAxis				The axis in which the slider works
	* @param	[in]	bRecenter			Use point of local activation as center (FALSE for the focus point)
	* @param	[in]	fHysteresisRatio	Additional size (from either side) it takes to exit a specific item.
	* @param	[in]	fPrimarySliderSize	Size of the slider (in mm)
	* @param	[in]	fFirstValue			Initial value when switching to the slider
	* @param	[in]	strName				Name of the control, for log purposes.
	*/
	XnVSelectableSlider1D(XnInt32 nCount, XnFloat fBorderWidth = 0, XnVAxis eAxis = AXIS_X,
		XnBool bRecenter = TRUE, 
		XnFloat fHysteresisRatio = XnVMultiItemHysteresis1D::ms_fDefaultHysteresisRatio,
		XnFloat fPrimarySliderSize = ms_fDefaultPrimarySliderSize, XnFloat fFirstValue = 0.5,
		const XnChar* strName = "XnVSelectableSlider1D");

	~XnVSelectableSlider1D();

	/**
	* Called when a primary point is created.
	*
	* @param	[in]	pContext	The hand context of the newly created primary point
	* @param	[in]	ptFocus		The point in which the session has started.
	*/
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptFocus);
	/**
	* Called when the primary point is updated.
	* This will cause the algorithm to manage the slider.
	*
	* @param	[in]	pContext	The hand context of the updated primary point
	*/
	void OnPrimaryPointUpdate(const XnVHandPointContext* pContext);
	/**
	 * Called when the primary point is destroyed.
	 *
	 * @param	[in]	nID	the ID of the last primary point
	 */
	void OnPrimaryPointDestroy(XnUInt32 nID);

	static const XnFloat ms_fDefaultPrimarySliderSize;	// = 250

	/**
	* Register for the hover event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterItemHover(void* cxt, ItemHoverCB pCB);
	/**
	* Register for the selection event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterItemSelect(void* cxt, ItemSelectCB pCB);
	/**
	* Register for the scroll event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterScroll(void* cxt, ScrollCB pCB);
	/**
	* Register for the value change event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterValueChange(void* cxt, ValueChangeCB pCB);
	/**
	* Register for the off-axis event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterOffAxisMovement(void* cxt, OffAxisMovementCB pCB);

	/**
	* Unregister from the hover event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterItemHover(XnCallbackHandle handle);
	/**
	* Unregister from the selection event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterItemSelect(XnCallbackHandle handle);
	/**
	* Unregister from the scroll event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterScroll(XnCallbackHandle handle);
	/**
	* Unregister from the value change event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterValueChange(XnCallbackHandle handle);
	/**
	* Unregister from the off-axis event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterOffAxisMovement(XnCallbackHandle handle);

	/**
	* Invoke the hover event
	*
	* @param	[in]	nItemIndex	The item that is hover
	*/
	void ItemHover(XnInt32 nItemIndex);
	/**
	* Invoke the selection event
	*
	* @param	[in]	nDirection	The direction of the selection
	*/
	void ItemSelect(XnVDirection nDirection);
	/**
	* Invoke the scroll event
	*
	* @param	[in]	fScrollValue	The current scroll value
	*/
	void Scroll(XnFloat fScrollValue);

	/**
	* Reposition the slider
	*
	* @param	[in]	ptCenter	The new center point for the slider.
	*/
	void Reposition(const XnPoint3D& ptCenter);
	void GetCenter(XnPoint3D& ptCenter) const;

	/**
	* Get the current number of items
	*
	* @return	The number of items
	*/
	XnUInt32 GetItemCount() const;
	/**
	 * Change the number of items
	 *
	 * @param	[in]	nItems	The new number of items
	 */
	void SetItemCount(XnUInt32 nItems);

	/**
	* Get the current border width
	*
	* @return	Current border width
	*/
	XnFloat GetBorderWidth() const;
	/**
	 * Change the border width
	 *
	 * @param	[in]	fWidth	New border width
	 */
	XnStatus SetBorderWidth(XnFloat fWidth);

	/**
	 * Get the size of the slider
	 * 
	 * @return	current size of the slider
	 */
	XnFloat GetSliderSize() const;
	/**
	 * Change the size of the slider
	 * 
	 * @param	[in]	fSliderSize	The new size of the slider
	 */
	void SetSliderSize(XnFloat fSliderSize);

	/**
	 * Change behavior: Should the SelectableSlider keep sending ValueChange events when checking OffAxis
	 *
	 * @param	[in]	bReport	New mode. default is off
	 */
	void SetValueChangeOnOffAxis(XnBool bReport);
	/**
	 * Get current behavior
	 */
	XnBool GetValueChangeOnOffAxis() const;

	/**
	 * Change the hysteresis ratio, to pass from one item to another.
	 * Units are the width of the cell.
	 *
	 * @param	[in]	fRatio	The new hysteresis ratio. 0 means no ratio, 0.5 means additional half width
	 */
	void SetHysteresisRatio(XnFloat fRatio);
	/**
	 * Get current hysteresis ratio
	 *
	 * @return	Current hysteresis ratio
	 */
	XnFloat GetHysteresisRatio() const;
protected:
	XnFloat GetFirstValue() const;

	XnFloat GetLeftScrollRegionEnd() const;
	XnFloat GetRightScrollRegionEnd() const;

	void PointCreated(const XnPoint3D& ptPos);
	void PointDeleted(XnBool bReason);
	void PointMoved(const XnPoint3D& ptPos, XnFloat fTime);

	void SetFirstValue(XnFloat fValue);

	void InternalCreateSlider(XnBool bPrimaryAxis, XnVAxis eAxis, const XnPoint3D& ptInitialPoint, XnFloat fInitialValue);

	// Is this needed public?
	void ValueChange(XnFloat fValue);
	void OffAxisMovement(XnVDirection dir);

	static void XN_CALLBACK_TYPE PrimarySlider_OffAxisMovement(XnVDirection eDir, void* pContext);
	static void XN_CALLBACK_TYPE PrimarySlider_ValueChange(XnFloat fValue, void* pContext);
	static void XN_CALLBACK_TYPE SecondarySlider_OffAxisMovement(XnVDirection eDir, void* pContext);
	static void XN_CALLBACK_TYPE SecondarySlider_ValueChange(XnFloat fValue, void* pContext);
	static void XN_CALLBACK_TYPE Hysteresis_ItemSelected(XnInt32 nItem, void* pContext);
	static void XN_CALLBACK_TYPE OffAxisHysteresis_ItemSelected(XnInt32 nItem, void* pContext);
	static void XN_CALLBACK_TYPE Scroller_Scrolled(XnFloat fScrollValue, void* pContext);

	void PrimarySlider_OffAxisMovement(XnVDirection eDir);
	void SecondarySlider_OffAxisMovement(XnVDirection eDir);
	void UpdatePrimarySlider(XnFloat fVal);
	void UpdateSecondarySlider(XnFloat fVal);
	void OffAxisHysteresis_ItemSelected(XnInt32 nItem);

	static const XnFloat ms_fSecondarySliderSize;	// = 150

	XnVMultiItemHysteresis1D* m_pHysteresis;
	XnVMultiItemHysteresis1D* m_pOffAxisHysteresis;
	XnVBorderScrollController1D* m_pScroller;

	XnBool m_bPositioned;
	XnBool m_bRecenter;

	XnBool m_bValueChangeOnOffAxis;

	XnVSlider1D* m_pSlider;				// Pointer to current active
	XnVSlider1D* m_pPrimarySlider;
	XnVSlider1D* m_pSecondarySlider;

	//Events this object is registered to:
	XnUInt32 m_nOffAxisHandle;
	XnUInt32 m_nValueChangeHandle;

	XnFloat m_fPrimarySliderSize;
	XnFloat m_fPrimarySliderValueAtChange;

	XnFloat m_fFirstValue;
	XnBool m_bFirstPointCreated;

	XnVAxis m_ePrimaryAxisDirection;
	XnUInt32 m_nItemCount;

	XnInt32 m_nLastHover;

	XnPoint3D m_ptCenter;

	XnCallbackHandle m_hPrimaryHysteresisItemSelect;
	XnCallbackHandle m_hSecondaryHysteresisItemSelect;
	XnCallbackHandle m_hScrollerScroll;

	XnCallbackHandle m_hPrimarySliderOffAxis;
	XnCallbackHandle m_hPrimarySliderValueChange;
	XnCallbackHandle m_hSecondarySliderOffAxis;
	XnCallbackHandle m_hSecondarySliderValueChange;

	XnVIntSpecificEvent m_ItemHoverCBs;
	XnVIntDirectionSpecificEvent m_ItemSelectCBs;
	XnVFloatSpecificEvent m_ScrollCBs, m_ValueChangeCBs;
	XnVDirectionSpecificEvent m_OffAxisMovementCBs;
}; // XnVSelectableSlider1D

#endif // _XNV_SELECTABLE_SLIDER_1D_H_
