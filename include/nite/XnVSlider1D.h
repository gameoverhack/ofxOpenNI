/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SLIDER_1D_H_
#define _XNV_SLIDER_1D_H_

#include "XnVNiteDefs.h"
#include "XnVDirection.h"
#include "XnVNiteEvents.h"

class XnVPointBuffer;

/**
* This is a simple slider. It receives a point,
* and normalizes it in one (configurable) axis to a number between 0 and 1.
* It can also recognize movement in its non-primary axis.
* The XnVSlider1D defines 2 events:
* - Its value has changed
* - A movement in its non-primary axis was detected.
*/
class XNV_NITE_API XnVSlider1D
{
public:
	/**
	* Type for the value change event callback. The value is between the slider's minimum and maximum output values.
	*/
	typedef void (XN_CALLBACK_TYPE* ValueChangeCB)(XnFloat fValue, void* cxt);
	/**
	* Type for the off axis movement event callback. It receives a direction of the off-axis movement.
	*/
	typedef void (XN_CALLBACK_TYPE* OffAxisMovementCB)(XnVDirection eDir, void* cxt);

	/**
	* Constructor
	*
	* @param	[in]	eAxis								The axis of the slider
	* @param	[in]	ptInitialPosition					The initial point within the slider
	* @param	[in]	ptMinPoint							One edge of the slider
	* @param	[in]	ptMaxPoint							Other edge of the slider
	* @param	[in]	fMinOutput							Minimum value for slider output
	* @param	[in]	fMaxOutput							Maximum value for slider output
	* @param	[in]	fOffAxisDetectionAngle				Angle from the slider to consider off-axis
	* @param	[in]	fOffAxisDetectionMinimumVelocity	Velocity of off-axis movement to consider off-axis
	*/
	XnVSlider1D(XnVAxis eAxis, const XnPoint3D& ptInitialPosition, XnPoint3D ptMinPoint, XnPoint3D ptMaxPoint,
		XnFloat fMinOutput, XnFloat fMaxOutput,
		XnFloat fOffAxisDetectionAngle = ms_fOffAxisDefaultDetectionAngle,
		XnFloat fOffAxisDetectionMinimumVelocity = ms_fDefaultMinimumOffAxisVelocity); 
	/**
	* Constructor
	*
	* @param	[in]	eAxis								The axis of the slider
	* @param	[in]	ptInitialPosition					The initial point within the slider
	* @param	[in]	fSliderLength						The length of the slider
	* @param	[in]	fInitialValue						The initial value to consider the initial point
	* @param	[in]	fMinOutput							Minimum value for slider output
	* @param	[in]	fMaxOutput							Maximum value for slider output
	* @param	[in]	fOffAxisDetectionAngle				Angle from the slider to consider off-axis
	* @param	[in]	fOffAxisDetectionMinimumVelocity	Velocity of off-axis movement to consider off-axis
	*/
	XnVSlider1D(XnVAxis eAxis, const XnPoint3D& ptInitialPosition, XnFloat fSliderLength, XnFloat fInitialValue,
		XnFloat fMinOutput, XnFloat fMaxOutput,
		XnFloat fOffAxisDetectionAngle = ms_fOffAxisDefaultDetectionAngle,
		XnFloat fOffAxisDetectionMinimumVelocity = ms_fDefaultMinimumOffAxisVelocity);

	~XnVSlider1D();

	/**
	* Recreate the slider
	*
	* @param	[in]	eAxis								The axis of the slider
	* @param	[in]	ptInitialPoint						The initial point within the slider
	* @param	[in]	ptMinPoint							One edge of the slider
	* @param	[in]	ptMaxPoint							Other edge of the slider
	* @param	[in]	fMinOutput							Minimum value for slider output
	* @param	[in]	fMaxOutput							Maximum value for slider output
	*/
	void Reinitialize(XnVAxis eAxis, const XnPoint3D& ptInitialPoint, const XnPoint3D& ptMinPoint,
		const XnPoint3D& ptMaxPoint, XnFloat fMinOutput, XnFloat fMaxOutput);
	/**
	* Recreate the slider
	*
	* @param	[in]	eAxis								The axis of the slider
	* @param	[in]	ptInitialPoint						The initial point within the slider
	* @param	[in]	fSliderLength						The length of the slider
	* @param	[in]	fInitialValue						The initial value to consider the initial point
	* @param	[in]	fMinOutput							Minimum value for slider output
	* @param	[in]	fMaxOutput							Maximum value for slider output
	*/
	void Reinitialize(XnVAxis eAxis, const XnPoint3D& ptInitialPoint, XnFloat fSliderLength,
		XnFloat fInitialValue, XnFloat fMinOutput, XnFloat fMaxOutput);

	/**
	* Translation between value and point.
	*/
	XnFloat ValueAtPosition(const XnPoint3D& pt);

	/**
	* The main function. It receives a point, normalizes it, and calculates the new value.
	*
	* @param	[in]	pt				The point to check in the slider.
	* @param	[in]	fTime			The timestamp (in seconds) of this update
	* @param	[in]	bCheckOffAxis	Is the OffAxis event interesting at all?
	*/
	XnStatus Update(const XnPoint3D& pt, XnFloat fTime, XnBool bCheckOffAxis = true);

	/**
	* Restart the search for off axis movement.
	*/
	void LostPoint();

	/**
	* Register for the value change event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterValueChange(void* cxt, ValueChangeCB CB);
	/**
	* Register for the off-axis event
	* 
	* @param	[in]	cxt	User's context
	* @param	[in]	CB	The Callback to call when the event is invoked.
	*
	* @return	A handle, to allow unregistration.
	*/
	XnCallbackHandle RegisterOffAxisMovement(void* cxt, OffAxisMovementCB CB);

	/**
	* Unregister from the value change event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterValueChange(XnCallbackHandle hCB);
	/**
	* Unregister from the off-axis event
	*
	* @param	[in]	hCB	The handle provided on registration.
	*/
	void UnregisterOffAxisMovement(XnCallbackHandle hCB);

	XnFloat GetValue() const;
	const XnPoint3D& GetPosition() const;
	XnBool IsValid() const;

	/**
	 * Get the axis on which the slider works
	 *
	 * @return	the axis
	 */
	XnVAxis GetAxis() const;

	/**
	 * Get the minimum velocity to consider Off Axis movement
	 *
	 * @return	The minimum velocity to consider OffAxis movement.
	 */
	XnFloat  GetOffAxisDetectionVelocity() const;
	/**
	 * Get the minimum angle to consider Off Axis movement
	 *
	 * @return The minimum angle to consider Off Axis movement
	 */
	XnFloat  GetOffAxisDetectionAngle() const;
	/**
	 * Get the time span in which an off axis movement should be identified
	 *
	 * @return The timespan in which an off axis movement should be identified.
	 */
	XnUInt32 GetOffAxisDetectionTime() const;

	/**
	 * Change the minimum velocity to consider Off Axis movement, in m/s. Default is 0.15m/s
	 *
	 * @param	[in]	fVelocity	New minimum velocity
	 */
	void SetOffAxisDetectionVelocity(XnFloat fVelocity);
	/**
	 * Change the minimum angle to consider Off Axis movement, in degrees. Default is 60
	 *
	 * @param	[in]	fAngle	New minimum angle
	 */
	void SetOffAxisDetectionAngle(XnFloat fAngle);
	/**
	 * Change the time span in which an Off Axis movement should be identified, in milliseconds. Default is 350ms
	 *
	 * @param	[in]	nTime	New time span
	 */
	void SetOffAxisDetectionTime(XnUInt32 nTime);
protected:
	void ValueChange(XnFloat fValue);
	void OffAxisMovement(XnVDirection eDir);

	XnVSlider1D(XnVAxis eAxis, XnBool bDraggable, const XnPoint3D& ptInitialPosition, XnFloat fSliderLength,
		XnFloat fInitialValue, XnFloat fMinOutput, XnFloat fMaxOutput,
		XnFloat fOffAxisDetectionAngle = ms_fOffAxisDefaultDetectionAngle,
		XnFloat fOffAxisDetectionMinimumVelocity = ms_fDefaultMinimumOffAxisVelocity);

	void InitializeFromPoint(const XnPoint3D& ptInitialPosition, const XnPoint3D& ptMinPoint, const XnPoint3D& ptMaxPoint,
		XnBool bVertical, XnFloat& fSliderLength, XnFloat& fInitialValue);
	void Initialize(XnVAxis eAxis, XnBool bDraggable, const XnPoint3D& ptInitialPosition, XnFloat fSliderLength,
		XnFloat fInitialValue, XnFloat fMinOutput, XnFloat fMaxOutput, XnFloat fOffAxisDetectionAngle,
		XnFloat fOffAxisDetectionMinimumMovement);

	XnVDirection CheckForOffAxisMovement(const XnPoint3D& pt, XnFloat fTime);
	XnInt32 CheckOffAxis(XnFloat fPrimaryAxisDelta, XnFloat fOffAxisDelta);


	static const XnFloat ms_fDefaultMinimumOffAxisVelocity;		// = 0.15f m/s
	static const XnFloat ms_fOffAxisDefaultDetectionAngle;			// = 60.0f
	static const XnUInt32 ms_nDefaultTimeForOffAxisDetection;		// = 350 ms

	XnFloat m_fOffAxisDetectionMinimumVelocity;
	XnFloat m_fOffAxisDetectionAngle;
	XnUInt32 m_nOffAxisDetectionTime;

	XnVAxis m_eAxis;
	XnBool m_bIsDraggable;

	XnFloat m_fCurrentOutput;
	XnPoint3D m_ptCurrentPosition;

	XnFloat m_fOutputMinimum, m_fOutputMaximum;
	XnFloat m_fMaxOutputMajorAxisPosition, m_fMinOutputMajorAxisPosition;

	XnVPointBuffer* m_pPointBuffer;

	XnVFloatSpecificEvent m_ValueChangeCBs;
	XnVDirectionSpecificEvent m_OffAxisMovementCBs;
};

#endif // _XNV_SLIDER_1D_H_
