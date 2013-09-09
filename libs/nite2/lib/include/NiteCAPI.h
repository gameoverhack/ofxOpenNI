/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0                                                        *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#include <OniPlatform.h>
#include <OniCAPI.h>
#include "NiteCTypes.h"
#include "NiteVersion.h"

#ifdef NITE_EXPORTS
#define NITE_API ONI_C_API_EXPORT
#else
#define NITE_API ONI_C_API_IMPORT
#endif

// General
NITE_API NiteStatus niteInitialize();
NITE_API void niteShutdown();

NITE_API NiteVersion niteGetVersion();

// UserTracker
NITE_API NiteStatus niteInitializeUserTracker(NiteUserTrackerHandle*);
NITE_API NiteStatus niteInitializeUserTrackerByDevice(void*, NiteUserTrackerHandle*);
NITE_API NiteStatus niteShutdownUserTracker(NiteUserTrackerHandle);

NITE_API NiteStatus niteStartSkeletonTracking(NiteUserTrackerHandle, NiteUserId);
NITE_API void niteStopSkeletonTracking(NiteUserTrackerHandle, NiteUserId);
NITE_API bool niteIsSkeletonTracking(NiteUserTrackerHandle, NiteUserId);

NITE_API NiteStatus niteSetSkeletonSmoothing(NiteUserTrackerHandle, float);
NITE_API NiteStatus niteGetSkeletonSmoothing(NiteUserTrackerHandle, float*);

NITE_API NiteStatus niteStartPoseDetection(NiteUserTrackerHandle, NiteUserId, NitePoseType);
NITE_API void niteStopPoseDetection(NiteUserTrackerHandle, NiteUserId, NitePoseType);
NITE_API void niteStopAllPoseDetection(NiteUserTrackerHandle, NiteUserId);

NITE_API NiteStatus niteRegisterUserTrackerCallbacks(NiteUserTrackerHandle, NiteUserTrackerCallbacks*, void*);
NITE_API void niteUnregisterUserTrackerCallbacks(NiteUserTrackerHandle, NiteUserTrackerCallbacks*);

NITE_API NiteStatus niteReadUserTrackerFrame(NiteUserTrackerHandle, NiteUserTrackerFrame**);

NITE_API NiteStatus niteUserTrackerFrameAddRef(NiteUserTrackerHandle, NiteUserTrackerFrame*);
NITE_API NiteStatus niteUserTrackerFrameRelease(NiteUserTrackerHandle, NiteUserTrackerFrame*);

// HandTracker
NITE_API NiteStatus niteInitializeHandTracker(NiteHandTrackerHandle*);
NITE_API NiteStatus niteInitializeHandTrackerByDevice(void*, NiteHandTrackerHandle*);
NITE_API NiteStatus niteShutdownHandTracker(NiteHandTrackerHandle);

NITE_API NiteStatus niteStartHandTracking(NiteHandTrackerHandle, const NitePoint3f*, NiteHandId* pNewHandId);
NITE_API void niteStopHandTracking(NiteHandTrackerHandle, NiteHandId);
NITE_API void niteStopAllHandTracking(NiteHandTrackerHandle);

NITE_API NiteStatus niteSetHandSmoothingFactor(NiteHandTrackerHandle, float);
NITE_API NiteStatus niteGetHandSmoothingFactor(NiteHandTrackerHandle, float*);

NITE_API NiteStatus niteRegisterHandTrackerCallbacks(NiteHandTrackerHandle, NiteHandTrackerCallbacks*, void*);
NITE_API void niteUnregisterHandTrackerCallbacks(NiteHandTrackerHandle, NiteHandTrackerCallbacks*);

NITE_API NiteStatus niteReadHandTrackerFrame(NiteHandTrackerHandle, NiteHandTrackerFrame**);

NITE_API NiteStatus niteHandTrackerFrameAddRef(NiteHandTrackerHandle, NiteHandTrackerFrame*);
NITE_API NiteStatus niteHandTrackerFrameRelease(NiteHandTrackerHandle, NiteHandTrackerFrame*);

NITE_API NiteStatus niteStartGestureDetection(NiteHandTrackerHandle, NiteGestureType);
NITE_API void niteStopGestureDetection(NiteHandTrackerHandle, NiteGestureType);
NITE_API void niteStopAllGestureDetection(NiteHandTrackerHandle);

NITE_API NiteStatus niteConvertJointCoordinatesToDepth(NiteUserTrackerHandle userTracker, float x, float y, float z, float* pX, float* pY);
NITE_API NiteStatus niteConvertDepthCoordinatesToJoint(NiteUserTrackerHandle userTracker, int x, int y, int z, float* pX, float* pY);
NITE_API NiteStatus niteConvertHandCoordinatesToDepth(NiteHandTrackerHandle handTracker, float x, float y, float z, float* pX, float* pY);
NITE_API NiteStatus niteConvertDepthCoordinatesToHand(NiteHandTrackerHandle handTracker, int x, int y, int z, float* pX, float* pY);

