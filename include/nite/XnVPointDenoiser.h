/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_POINT_DENOISER_H_
#define _XNV_POINT_DENOISER_H_

#include "XnVPointFilter.h"

struct XnVDenoisingLocalContext;
struct XnVIntLocalHash;

/**
* The XnVPointDenoiser holds its own XnVMultipleHands object.
* It receives notifications from earlier nodes, and sends on its own notifications
* with matching IDs.
* The local points are denoised.
*/
class XNV_NITE_API XnVPointDenoiser :
	public XnVPointFilter
{
public:
	/**
	 * Constructor
	 *
	 * @param	[in]	fDistanceThreshold		Distance (in x-y) to change between close and far ratios
	 * @param	[in]	strName					Name of the control, for log purposes.
	 */
	XnVPointDenoiser(XnFloat fDistanceThreshold = ms_fDefaultDistanceThreshold,
		const XnChar* strName = "XnVPointDenoiser");
	~XnVPointDenoiser();

	/**
	 * Handle the current state
	 *
	 * @param	pMessage	The current state
	 */
	void Update(XnVMessage* pMessage);

	/**
	 * Handle a new point created
	 *
	 * @param	[in]	pContext	The new hand created
	 */
	void OnPointCreate(const XnVHandPointContext* pContext);
	/**
	 * Handle an existing point update
	 *
	 * @param	[in]	pContext	the new details of an existing point
	 */
	void OnPointUpdate(const XnVHandPointContext* pContext);
	/**
	 * Handle an existing point destruction
	 *
	 * @param	[in]	nID	The Id of the destroyed point
	 */
	void OnPointDestroy(XnUInt32 nID);
	/**
	 * Update the internal points' focus point
	 *
	 * @param	[in]	pContext	The hand point that creates the session
	 * @param	[in]	ptSessionStarter	The position in which the session has started
	 */
	void OnPrimaryPointCreate(const XnVHandPointContext* pContext, const XnPoint3D& ptSessionStarter);

	/**
	 * Get the current noise threshold
	 *
	 * @return	Current noise threshold
	 */
	XnFloat GetDistanceThreshold() const;
	/**
	* Change distance limit for far and close ratios
	*
	* @param	fDistanceThreshold	The new threshold
	*/
	void SetDistanceThreshold(XnFloat fDistanceThreshold);

	XnFloat GetCloseRatio() const;
	XnFloat GetFarRatio() const;

	/**
	 * Change the ratio for close points between the new point and the old ones.
	 *
	 * @param	[in]	fCloseRatio	The new ratio for close points
	 */
	void SetCloseRatio(XnFloat fCloseRatio);
	/**
	 * Change the ratio for far points between the new point and the old ones.
	 *
	 * @param	[in]	fFarRatio	The new ratio for far points
	 */
	void SetFarRatio(XnFloat fFarRatio);

protected:
	XnVDenoisingLocalContext* GetLocalContext(XnUInt32 nID);
	XnFloat Distance(XnPoint3D& pt1, XnPoint3D& pt2) const;
	void UpdatePointDenoise(XnPoint3D& ptToChange, const XnPoint3D& ptDontChange);

	void Clear();

	XnVIntLocalHash* m_pActivePoints;

	static const XnFloat ms_fDefaultDistanceThreshold;	//	= 10
	static const XnFloat ms_fDefaultCloseRatio;		// = 0.0
	static const XnFloat ms_fDefaultFarRatio;			// = 1.0

	XnFloat m_fDistanceThreshold;
	XnFloat m_fCloseRatio;
	XnFloat m_fFarRatio;

	XnVMultipleHands m_DenoisedHands;
};

#endif
