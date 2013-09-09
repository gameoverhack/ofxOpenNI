/*******************************************************************************
*                                                                              *
*   PrimeSense  2.0                                                        *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef _NITE_ENUMS_H_
#define _NITE_ENUMS_H_

/** Available joints in skeleton */
typedef enum
{
	JOINT_HEAD,
	JOINT_NECK,

	JOINT_LEFT_SHOULDER,
	JOINT_RIGHT_SHOULDER,
	JOINT_LEFT_ELBOW,
	JOINT_RIGHT_ELBOW,
	JOINT_LEFT_HAND,
	JOINT_RIGHT_HAND,

	JOINT_TORSO,

	JOINT_LEFT_HIP,
	JOINT_RIGHT_HIP,
	JOINT_LEFT_KNEE,
	JOINT_RIGHT_KNEE,
	JOINT_LEFT_FOOT,
	JOINT_RIGHT_FOOT,
} JointType;

/** Possible states of the skeleton */
typedef enum
{
	/** No skeleton - skeleton was not requested */
	SKELETON_NONE,
	/** Skeleton requested, but still unavailable */
	SKELETON_CALIBRATING,
	/** Skeleton available */
	SKELETON_TRACKED,

	/** Possible reasons as to why skeleton is unavailable */
	SKELETON_CALIBRATION_ERROR_NOT_IN_POSE,
	SKELETON_CALIBRATION_ERROR_HANDS,
	SKELETON_CALIBRATION_ERROR_HEAD,
	SKELETON_CALIBRATION_ERROR_LEGS,
	SKELETON_CALIBRATION_ERROR_TORSO

} SkeletonState;

/** Possible failure values */
typedef enum
{
	STATUS_OK,
	STATUS_ERROR,
	STATUS_BAD_USER_ID,
	STATUS_OUT_OF_FLOW
} Status;

typedef enum
{
	POSE_PSI,
	POSE_CROSSED_HANDS
} PoseType;

/** Available gestures types */
typedef enum
{
	GESTURE_WAVE,
	GESTURE_CLICK,
	GESTURE_HAND_RAISE
} GestureType;

#endif // _NITE_ENUMS_H_