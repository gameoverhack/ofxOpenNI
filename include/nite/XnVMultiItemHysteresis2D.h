/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_MULTI_ITEM_HYSTERESIS_2D_H_
#define _XNV_MULTI_ITEM_HYSTERESIS_2D_H_

#include "XnVNiteDefs.h"
#include "XnVNiteEvents.h"

class XnVSigmoid;
class XnVPointBuffer;

/**
* The multi item controller is initialized with a number of items in each axis,
* and sends a select event when moving between them.
* On update it gets a number between 0 and 1 for each axis,
* and sees which cell that translates into.
* Selection here means being over the item.
* The XnVMultiItemController defines one events:
* - An item was selected
*
* A hysteresis ratio is used to 'enlarge' the current item.
*/
class XNV_NITE_API XnVMultiItemHysteresis2D
{
public:
	/**
	* Type for the selection event callback
	*/
	typedef void (XN_CALLBACK_TYPE *ItemSelectCB)(XnInt32 ItemXIndex, XnInt32 ItemYIndex, void* pUserCxt);

	/**
	* Constructor
	*
	* @param	[in]	nItemXCount			Horizontal items
	* @param	[in]	nItemYCount			Vertical items
	* @param	[in]	fBorderWidth		Size from all sides to disregard
	* @param	[in]	fHysteresisRatio	Additional size (from any side) it takes to exit a specific item.
	*/
	XnVMultiItemHysteresis2D(XnInt32 nItemXCount, XnInt32 nItemYCount,
		XnFloat fBorderWidth = 0,
		XnFloat fHysteresisRatio = ms_fDefaultHysteresisRatio);

	~XnVMultiItemHysteresis2D();

	/**
	* Update a value between 0 and 1 for each axis,
	* which will be translated into an indices of the current cell.
	*/
	XnStatus Update(XnFloat fXValue, XnFloat fYValue);
	/**
	* Lose the point. Clear internal buffer.
	*/
	void LostPoint();

	/**
	* Invoke the selection event
	*
	* @param	[in]	nItemXIndex	The X-coordinate of the item that is selected
	* @param	[in]	nItemYIndex	The Y-coordinate of the item that is selected
	*/
	void ItemSelected(XnInt32 nItemXIndex, XnInt32 nItemYIndex);

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
	* Unregister from the selection event
	*
	* @param	[in]	handle	The handle provided on registration.
	*/
	void UnregisterItemSelect(XnCallbackHandle handle);

	/**
	* Get the current number of item rows
	*
	* @return	The current number of rows
	*/
	XnUInt32 GetItemXCount() const;
	/**
	* Change the current number of items rows
	*
	* @param	[in]	nItemXCount	New number of rows
	*/
	void SetItemXCount(XnUInt32 nItemXCount);
	/**
	* Get the current number of item columns
	*
	* @return	The current number of columns
	*/
	XnUInt32 GetItemYCount() const;
	/**
	* Change the current number of items columns
	*
	* @param	[in]	nItemYCount	New number of columns
	*/
	void SetItemYCount(XnUInt32 nItemYCount);

	/**
	* Change the current number of items rows and columns
	*
	* @param	[in]	nItemXCount	New number of rows
	* @param	[in]	nItemYCount	New number of columns
	*/
	void SetItemCount(XnUInt32 nItemXCount, XnUInt32 nItemYCount);

	/**
	* Get the minimal value the hysteresis expects in the X-coordinate
	*
	* @return	The minimal value
	*/
	XnFloat GetMinXValue() const;
	/**
	* Get the minimal value the hysteresis expects in the Y-coordinate
	*
	* @return	The minimal value
	*/
	XnFloat GetMinYValue() const;
	/**
	* Get the maximal value the hysteresis expects in the X-coordinate
	*
	* @return	The maximal value
	*/
	XnFloat GetMaxYValue() const;
	/**
	* Get the maximal value the hysteresis expects in the Y-coordinate
	*
	* @return	The maximal value
	*/
	XnFloat GetMaxXValue() const;

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
	* Get the ratio for an item selection to change.
	* 
	* @return	The ratio
	*/
	XnFloat GetHysteresisRatio() const;

	void SetHysteresisRatio(XnFloat fRatio);

	static const XnFloat ms_fDefaultHysteresisRatio;	// = 0.5f
protected:
	void CalculateMinAndMax();

	XnBool IsValueInRange(XnFloat fXValue, XnFloat fYValue);

	XnUInt32 m_nItemXCount;
	XnUInt32 m_nItemYCount;

	XnFloat m_fMinXValue, m_fMaxXValue;
	XnFloat m_fMinYValue, m_fMaxYValue;

	XnInt32 m_nLastSelectedXIndex;
	XnInt32 m_nLastSelectedYIndex;

	XnFloat m_fBorderWidth;

	XnFloat m_fHysteresisRatio;

	XnVIntIntSpecificEvent m_ItemSelectCBs;
}; // XnVMultiItem2DController

#endif // _XNV_MULTI_ITEM_HYSTERESIS_1D_H_
