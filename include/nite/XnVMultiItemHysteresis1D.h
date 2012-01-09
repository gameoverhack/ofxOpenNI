/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MULTI_ITEM_HYSTERESIS_1D_H_
#define _XNV_MULTI_ITEM_HYSTERESIS_1D_H_

#include "XnVNiteDefs.h"

class XnVIntSpecificEvent;
/**
* The multi item hysteresis is initialized with a number of items,
* and sends a select event when moving between them.
* On update it gets a number between 0 and 1, and sees which cell that translates into.
* Selection here means being over the item.
* The XnVMultiItemHysteresis1D defines one events:
* - An item was selected
*
* A hysteresis ratio is used to 'enlarge' the current item.
*/
class XNV_NITE_API XnVMultiItemHysteresis1D
{
public:
	/**
	* Type for the selection event callback
	*/
	typedef void (XN_CALLBACK_TYPE *ItemSelectedCB)(XnInt32 ItemIndex, void* pUserCxt);

	/**
	* Constructor
	*
	* @param	[in]	nItemCount			Number of items
	* @param	[in]	fBorderWidth		Size to disregard from either side
	* @param	[in]	fHysteresisRatio	Additional size (from either side) it takes to exit a specific item.
	*/
	XnVMultiItemHysteresis1D(XnInt32 nItemCount, XnFloat fBorderWidth = 0,
		XnFloat fHysteresisRatio = ms_fDefaultHysteresisRatio);

	~XnVMultiItemHysteresis1D();

	/**
	* Update a value between 0 and 1, which will be translated into an index of the current cell.
	*/
	XnStatus Update(XnFloat Value);
	/**
	 * Lose the point. Clear internal buffer.
	 */
	void LostPoint();

	/**
	 * Invoke the selection event
	 *
	 * @param	[in]	nItemIndex	The item that is selected
	 */
	void ItemSelect(XnInt32 nItemIndex);

	/**
	* Register for the selection event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	pCB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterItemSelect(void* cxt, ItemSelectedCB pCB);
	/**
	* Unregister from the selection event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterItemSelect(XnCallbackHandle handle);

	/**
	 * Get the border width - the space on either side that isn't part of any item.
	 *
	 * @return	The current border width
	 */
	XnFloat GetBorderWidth() const;
	/**
	 * Change the border width
	 *
	 * @param	[in]	fWidth	The new border width
	 */
	XnStatus SetBorderWidth(XnFloat fWidth);

	/**
	 * Get the current number of items.
	 *
	 * @return	The current number of items
	 */
	XnUInt32 GetItemCount() const;
	/**
	 * Change the current number of items
	 *
	 * @param	[in]	nCount	New number of items
	 */
	void SetItemCount(XnUInt32 nCount);

	/**
	 * Get the minimal value the hysteresis expects.
	 *
	 * @return	The minimal value
	 */
	XnFloat GetMinValue() const;
	/**
	* Get the maximal value the hysteresis expects.
	*
	* @return	The maximal value
	*/
	XnFloat GetMaxValue() const;

	/**
	 * Get the ratio for an item selection to change.
	 * 
	 * @return	The ratio
	 */
	XnFloat GetHysteresisRatio() const;

	/**
	 * Change the hysteresis ratio
	 *
	 * @param	[in]	fRatio	New hysteresis ratio
	 */
	void SetHysteresisRatio(XnFloat fRatio);
	static const XnFloat ms_fDefaultHysteresisRatio;	// = 0.5f
protected:
	void CalculateMinAndMax();
	bool IsValueInRange(XnFloat Value);


	XnUInt32 m_nItemCount;
	XnFloat m_fMinValue;
	XnFloat m_fMaxValue;

	XnInt32 m_nLastSelectedIndex;

	XnFloat m_fBorderWidth;
	XnFloat m_fHysteresisRatio;

	XnVIntSpecificEvent* m_pItemSelectCBs;
};

#endif // _XNV_MULTI_ITEM_HYSTERESIS_1D_H_
