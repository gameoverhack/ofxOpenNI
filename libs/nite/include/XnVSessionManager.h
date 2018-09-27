/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_SESSION_MANAGER_H_
#define _XNV_SESSION_MANAGER_H_

#include "XnVDeviceControl.h"
#include "XnVMessageGenerator.h"
#include "XnVMultipleHands.h"
#include "XnVGesture.h"
#include "XnVPointTracker.h"
#include "XnVPointMessage.h"
#include "XnVSessionGenerator.h"
#include "XnV3DVector.h"
#include "XnVSteadyDetector.h"

class XnVGestureHash;
/**
* A XnVSessionManager manages the session state
* Possible states are:
* Not In Session - Looking for the main gesture. Changing to this state is a "SessionStop" event.
* In Session - Tracking hand points. Changing to this state is a "SessionStart" event.
* Quick refocus - lost last point, looking for it where it disappeared, and for main gesture
*
* Initial mode is "Not in session"
*/
class XNV_NITE_API XnVSessionManager :
	public XnVContextControl,
	public XnVSessionGenerator
{
public:
	/**
	* Constructor. Create a new Session Manager
	*
	* @param	[in]	strName	A name, for Log use
	*/
	XnVSessionManager(const XnChar* strName = "XnVSessionManager");
	~XnVSessionManager();

	/**
	* Initialize the Session. Set the gestures to use for focus and for quick refocus, as two comma-separated lists.
	* 
	* @param	[in]	pContext		The OpenNI context from which to take some values
	* @param	[in]	strUseAsFocus	A comma-separated list of gestures to use as the main focus gesture
	* @param	[in]	strUseAsQuickRefocus	A comma-separated list of gesture to use as the quick focus gesture
	* @param	[in]	pTracker		The HandsGenerator to use for tracking
	* @param	[in]	pFocusGenerator	The GestureGenerator from which to take the focus gesture
	* @param	[in]	pQuickRefocusGenerator	The GestureGenerator from which to take the quick refocus gesture
	*
	* @return	XN_STATUS_OK on success
	*/
	XnStatus Initialize(xn::Context* pContext,
						const XnChar* strUseAsFocus, const XnChar* strUseAsQuickRefocus,
						xn::HandsGenerator* pTracker = NULL, xn::GestureGenerator* pFocusGenerator = NULL,
						xn::GestureGenerator* pQuickRefocusGenerator = NULL);

	/**
	* Handle incoming Messages.
	* Depth Messages are dealt with by the other Update method
	* Point Messages (from Point Tracker) are dealt with by ProcessPoints
	*
	* @param	[in]	pMessage	incoming Message
	*/
	void Update(XnVMessage* pMessage);
	/**
	* Handle a context. Calls the Gesture and the Tracker
	*
	* @param	[in]	pContext	The context to process
	*/
	void Update(const xn::Context* pContext);
	/**
	* Handle Hand Points. Manage session state.
	*
	* @param	[in]	pHands	Points summary
	*/
	void ProcessPoints(XnVMultipleHands* pHands);

	/**
	* Replace Tracker used
	*
	* @param	[in]	pTracker	new Tracker
	*/
	void SetTracker(XnVPointTracker* pTracker);
	void SetTracker(xn::HandsGenerator generator);

	/**
	* Change the timeout for the quick refocus gesture. Default is 15000 (15 seconds)
	*
	* @param	[in]	nMilliseconds	The new timeout
	*/
	void SetQuickRefocusTimeout(XnUInt32 nMilliseconds);
	XnUInt32 GetQuickRefocusTimeout() const;

	/**
	* Stop tracking a specific point
	*
	* @param	[in]	nID	The ID of the point to stop track
	*/
	XnStatus LosePoint(XnUInt32 nID);
	/**
	* Stop tracking all points
	*/
	XnStatus LosePoints();

	/**
	 * Stop the current session. All points will be lost.
	 */
	XnStatus EndSession();
	/**
	 * Force a session to start, with a specific focus position.
	 *
	 * @param	[in]	ptFocus	The position to consider as focus point
	 */
	XnStatus ForceSession(const XnPoint3D& ptFocus);
	/**
	 * Start tracking a new hand from a specific position
	 *
	 * @param	[in]	pt	The position from which to start tracking
	 */
	XnStatus TrackPoint(const XnPoint3D& pt);

	/**
	 * Clear the multi-thread queue
	 */
	void ClearQueue();

	/**
	 * Set the area in which Quick Refocus is expected
	 *
	 * @param	[in]	bbQuickRefocusArea	Area relative to the last known hand (treated as 0,0,0) to expect quick refocus
	 */
	XnStatus SetQuickRefocusArea(const XnBoundingBox3D& bbQuickRefocusArea);
	/**
	* Set the area in which Quick Refocus is expected, where all lengths are symmetric relative to the last known point
	*
	* @param	[in]	fLengthX	Length in the X coordinate, half of which will be in either direction
	* @param	[in]	fLengthY	Length in the Y coordinate, half of which will be in either direction
	* @param	[in]	fLengthZ	Length in the z coordinate, half of which will be in either direction
	*/
	XnStatus SetQuickRefocusArea(XnFloat fLengthX, XnFloat fLengthY, XnFloat fLengthZ);

	void GetQuickRefocusArea(XnFloat& fLengthX, XnFloat& fLengthY, XnFloat& fLengthZ);
	/**
	 * Force the system into quick refocus state, if not already in a session.
	 *
	 * @param	[in]	pbbForcedArea	A fixed area in which to look for the quick refocus. This overrides but doesn't overwrite the regular Quick Refocus area.
	 * @param	[in]	nForcedTimeout	Timeout in ms in which to look for quick refocus. This overrides but doesn't overwrite the regular Quick refocus timeout.
	 */
	void ForceQuickRefocus(XnBoundingBox3D* pbbForcedArea, XnUInt32 nForcedTimeout);

	/**
	 * Add user-defined gesture to be used as a focus gesture
	 *
	 * @param	[in]	pGesture	User-defined gesture
	 *
	 * @return	ID to allow removal
	 */
	XnUInt32 AddGesture(XnVGesture* pGesture);
	/**
	* Add gesture to be used as a focus gesture, by gesture generator (OpenNI) and name
	*
	* @param	[in]	generator	Generator with the wanted gesture
	* @param	[in]	strName		Name of the wanted gesture in the generator
	*
	* @return	ID to allow removal
	*/
	XnUInt32 AddGesture(xn::GestureGenerator& generator, const XnChar* strName);
	/**
	* Add user-defined gesture to be used as a focus gesture, by OpenNI context and name
	*
	* @param	[in]	context		OpenNI context
	* @param	[in]	strName		Name of gesture, to be searched in all gesture generators
	*
	* @return	ID to allow removal
	*/
	XnUInt32 AddGesture(xn::Context& context, const XnChar* strName);
	/**
	 * Remove a gesture from being a focus gesture
	 *
	 * @param	[in]	nId		ID of the gesture to be removed, as received when adding it.
	 */
	void RemoveGesture(XnUInt32 nId);

	/**
	* Add user-defined gesture to be used as a quick refocus gesture
	*
	* @param	[in]	pGesture	User-defined gesture
	*
	* @return	ID to allow removal
	*/
	XnUInt32 AddQuickRefocus(XnVGesture* pGesture);
	/**
	* Add gesture to be used as a quick refocus gesture, by gesture generator (OpenNI) and name
	*
	* @param	[in]	generator	Generator with the wanted gesture
	* @param	[in]	strName		Name of the wanted gesture in the generator
	*
	* @return	ID to allow removal
	*/
	XnUInt32 AddQuickRefocus(xn::GestureGenerator& generator, const XnChar* strName);
	/**
	* Add user-defined gesture to be used as a quick refocus gesture, by OpenNI context and name
	*
	* @param	[in]	context		OpenNI context
	* @param	[in]	strName		Name of gesture, to be searched in all gesture generators
	*
	* @return	ID to allow removal
	*/
	XnUInt32 AddQuickRefocus(xn::Context& context, const XnChar* strName);
	/**
	* Remove a gesture from being a quick refocus gesture
	*
	* @param	[in]	nId		ID of the gesture to be removed, as received when adding it.
	*/
	void RemoveQuickRefocus(XnUInt32 nId);

	/**
	 * Turn on looking for gestures when the primary hand is static
	 */
	void StartPrimaryStatic();
	/**
	 * Turn off looking for gestures when the primary hand is static
	 */
	void StopPrimaryStatic();
	/**
	 * Set the timeout of the primary hand being static to allow gestures
	 *
	 * @param	[in]	fTimeout	Time in seconds
	 */
	void SetPrimaryStaticTimeout(XnFloat fTimeout);
	/**
	 * Get the timeout of the primary hand being static to allow gestures
	 *
	 * @return Time in seconds
	 */
	XnFloat GetPrimarySteadyTimeout() const;

	/**
	 * Set the box in which to look for the gesture as a result of the primary hand being static.
	 * The box will be around the last focus point, with the focus point being its center.
	 *
	 * @param	[in]	fX	Length (in mm) on the X-axis
	 * @param	[in]	fY	Length (in mm) on the Y-axis
	 * @param	[in]	fZ	Length (in mm) on the Z-axis
	 */
	void SetPrimaryStaticBoundingBox(XnFloat fX, XnFloat fY, XnFloat fZ);
	/**
	 * Get the box dimensions in which a gesture is looked for when the primary hand is static
	 *
	 * @param	[out]	fX	Length (in mm) in the X-axis
	 * @param	[out]	fY	Length (in mm) in the Y-axis
	 * @param	[out]	fZ	Length (in mm) in the Z-axis
	 */
	void GetPrimaryStaticBoundingBox(XnFloat& fX, XnFloat& fY, XnFloat& fZ);

	XnStatus XN_API_DEPRECATED("Please use Initialize() instead.")
		Initialize(XnVGesture* pFocusGesture, XnVGesture* pQuickRefocusGesture, XnVPointTracker* pTracker);

	void XN_API_DEPRECATED("Please use AddGesture() instead.")
		SetGesture(XnVGesture* pGesture);
	void XN_API_DEPRECATED("Please use AddGesture() instead.")
		SetGesture(xn::GestureGenerator gestureGenerator, const XnChar* strGestures);

	void XN_API_DEPRECATED("Please use AddQuickRefocus() instead.")
		SetQRGesture(XnVGesture* pGesture);
	void XN_API_DEPRECATED("Please use AddQuickRefocus() instead.")
		SetQRGesture(xn::GestureGenerator gestureGenerator, const XnChar* strGestures);

protected:

	void SessionStop();
	void StartQuickRefocus(XnBoundingBox3D* pbbROI, XnUInt32 nTimeout);

	XnStatus FindTracker(xn::Context* pContext, xn::HandsGenerator* pTracker, xn::HandsGenerator& tracker);

	static void XN_CALLBACK_TYPE Gesture_Recognized(const XnChar* strGesture, const XnPoint3D& ptIDPosition, const XnPoint3D& ptEndPosition, void* cxt);
	static void XN_CALLBACK_TYPE Gesture_StartRecognized(const XnChar* strGesture, const XnPoint3D& pos, XnFloat fProgress, void* cxt);
	void GestureRecognized(const XnChar* strGesture, const XnPoint3D& ptIDPosition, const XnPoint3D& ptEndPosition);
	void GestureStartRecognized(const XnChar* strGesture, const XnPoint3D& pos, XnFloat fProgress);

	XnUInt64 GetTime();

	void EnableGestures(XnBoundingBox3D* pROI = NULL);
	void EnableQuickRefocusGestures(XnBoundingBox3D* pROI = NULL);
	void DisableGestures();
	void DisableQuickRefocusGestures();
	void UpdateGestures(const xn::Context* pContext);
	void UpdateQuickRefocusGestures(const xn::Context* pContext);

	XnBool m_bGesturesEnabled;
	XnBool m_bQuickRefocusEnabled;

	void ClearGestureHash(XnVGestureHash& hash);
	XnVGestureHash* m_pMainGestures;
	XnVGestureHash* m_pQuickRefocusGestures;
	XnUInt32 m_nNextGestureID;

	XnUInt32 AddGesture(XnVGesture* pGesture, XnBool bAuto, XnUInt32 nHintID, XnBool bFocus);

	XnUInt32 AddGesture(xn::Context& context, const XnChar* strName, const XnStringsHash& hash, XnBool bFocus);
	XnUInt32 AddGesture(xn::GestureGenerator& generator, const XnChar* strName, const XnStringsHash& hash, XnBool bFocus);

	void RemoveGesture(XnUInt32 nId, XnVGestureHash& hash);

	XnVPointTracker* m_pTracker;
	XnBool m_bAutoTracker;

	enum
	{
		SMGR_ILLEGAL		= 0x0000,
		SMGR_NOT_IN_SESSION	= 0x0001,
		SMGR_IN_SESSION     = 0x0002,
		SMGR_QUICK_REFOCUS	= SMGR_IN_SESSION | SMGR_NOT_IN_SESSION,
		SMGR_START_SESSION	= 0x0004,
	} m_eSessionState;

	inline XnBool IsInSession() const {return (m_eSessionState & SMGR_IN_SESSION) != 0;}
	inline XnBool IsNotInSession() const {return (m_eSessionState & SMGR_NOT_IN_SESSION) != 0;}
	inline XnBool IsQuickRefocus() const {return m_eSessionState == SMGR_QUICK_REFOCUS;}

	XnBool m_bQuickRefocusAllowed;

	XnCallbackHandle m_hGesture;
	XnCallbackHandle m_hGestureMid;
	XnCallbackHandle m_hQRGesture;

	XnUInt32 m_nQuickRefocusTimeout;
	XnUInt64 m_nQuickRefocusExpire;

	static const XnUInt32 ms_nQuickRefocusDefaultTimeout;

	XnV3DVector m_ptLastPrimaryPosition;

	XnBoundingBox3D m_bbQuickRefocusArea;
	const xn::Context* m_pContext;

	XnBool m_bCheckPrimaryForStatic;
	XnBool m_bPrimaryIsStatic;
	XnVSteadyDetector* m_pSteadyDetector;

	static void XN_CALLBACK_TYPE PrimaryIsSteady(XnUInt32 id, XnFloat dummy, void* cxt);
	static void XN_CALLBACK_TYPE PrimaryIsNotSteady(XnUInt32 id, XnFloat dummy, void* cxt);

	XnFloat m_fLastSteadyTime;
	XnFloat m_fLastPrimaryTime;
	XnFloat m_fSteadyTimeout;

	XnUInt32 m_nLastPrimaryID;
	XnPoint3D m_GestureWhileTrackingThreshold;

	void InitializeStatic();
	void FinalizeStatic();
};

#endif
