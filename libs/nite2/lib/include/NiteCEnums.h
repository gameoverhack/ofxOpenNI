/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0                                                        *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef _NITE_C_ENUM_H_
#define _NITE_C_ENUM_H_

/** Available joints in skeleton */
typedef enum
{
	NITE_JOINT_HEAD,
	NITE_JOINT_NECK,

	NITE_JOINT_LEFT_SHOULDER,
	NITE_JOINT_RIGHT_SHOULDER,
	NITE_JOINT_LEFT_ELBOW,
	NITE_JOINT_RIGHT_ELBOW,
	NITE_JOINT_LEFT_HAND,
	NITE_JOINT_RIGHT_HAND,

	NITE_JOINT_TORSO,

	NITE_JOINT_LEFT_HIP,
	NITE_JOINT_RIGHT_HIP,
	NITE_JOINT_LEFT_KNEE,
	NITE_JOINT_RIGHT_KNEE,
	NITE_JOINT_LEFT_FOOT,
	NITE_JOINT_RIGHT_FOOT,
} NiteJointType;

/** Possible states of the skeleton */
typedef enum
{
	/** No skeleton - skeleton was not requested */
	NITE_SKELETON_NONE,
	/** Skeleton requested, but still unavailable */
	NITE_SKELETON_CALIBRATING,
	/** Skeleton available */
	NITE_SKELETON_TRACKED,

	/** Possible reasons as to why skeleton is unavailable */
	NITE_SKELETON_CALIBRATION_ERROR_NOT_IN_POSE,
	NITE_SKELETON_CALIBRATION_ERROR_HANDS,
	NITE_SKELETON_CALIBRATION_ERROR_HEAD,
	NITE_SKELETON_CALIBRATION_ERROR_LEGS,
	NITE_SKELETON_CALIBRATION_ERROR_TORSO

} NiteSkeletonState;

/** Possible states of the user */
typedef enum
{
	/** User is visible and already known */
	NITE_USER_STATE_VISIBLE = 1,
	/** User is new - this is the first time the user is available */
	NITE_USER_STATE_NEW = 2,
	/** User is lost. This is the last time this user will be seen */
	NITE_USER_STATE_LOST = 4,
} NiteUserState;

/** Possible failure values */
typedef enum
{
	NITE_STATUS_OK,
	NITE_STATUS_ERROR,
	NITE_STATUS_BAD_USER_ID,
	NITE_STATUS_OUT_OF_FLOW
} NiteStatus;

typedef enum
{
	NITE_POSE_PSI,
	NITE_POSE_CROSSED_HANDS
} NitePoseType;

typedef enum
{
	NITE_POSE_STATE_DETECTING = 1,
	NITE_POSE_STATE_IN_POSE = 2,
	NITE_POSE_STATE_ENTER = 4,
	NITE_POSE_STATE_EXIT = 8

} NitePoseState;

/** Available gestures types */
typedef enum
{
	NITE_GESTURE_WAVE,
	NITE_GESTURE_CLICK,
	NITE_GESTURE_HAND_RAISE
} NiteGestureType;

/** Possible state of a gesture. Currently only 'Complete' is used. */
typedef enum
{
	NITE_GESTURE_STATE_NEW = 1,				// Future
	NITE_GESTURE_STATE_IN_PROGRESS = 2,		// Future
	NITE_GESTURE_STATE_COMPLETED = 4
} NiteGestureState;

/** Possible state of a hand */
typedef enum
{
 	/** This hand was lost. It is the last frame in which it will be provided */
	NITE_HAND_STATE_LOST = 0,
	/** This is a new hand - it is the first frame in which it is available*/
	NITE_HAND_STATE_NEW = 1,
 	/** This is a known hand */
	NITE_HAND_STATE_TRACKED = 2,
 	/** This is a known hand, and in this frame it's very near the edge of the field of view */
	NITE_HAND_STATE_TOUCHING_FOV = 4,
} NiteHandState;

#endif // _NITE_C_ENUM_H_