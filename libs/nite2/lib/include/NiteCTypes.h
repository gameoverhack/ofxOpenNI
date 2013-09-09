/*******************************************************************************
*                                                                              *
*   PrimeSense NiTE 2.0                                                        *
*   Copyright (C) 2012 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef _NITE_C_TYPES_H_
#define _NITE_C_TYPES_H_

#include <OniCTypes.h>
#include "NiteCEnums.h"

typedef short int NiteUserId;

typedef struct NiteUserTracker* NiteUserTrackerHandle;

#define NITE_JOINT_COUNT 15
#define NITE_POSE_COUNT 2

/** 3D Point */
typedef struct
{
	float x, y, z;
} NitePoint3f;

/** Quaternion */
typedef struct 
{
	float x, y, z, w;
} NiteQuaternion;

/** Single joint of a skeleton */
typedef struct  
{
	/** Type of the joint */
	NiteJointType jointType;

	/** Position of the joint - in real world coordinates */
	NitePoint3f position;
	float positionConfidence;

	/** Orientation of the joint */
	NiteQuaternion orientation;
	float orientationConfidence;
} NiteSkeletonJoint;

/** 3D Box */
typedef struct  
{
	NitePoint3f min;
	NitePoint3f max;
} NiteBoundingBox;

typedef struct
{
	NitePoseType type;
	int state;
} NitePoseData;

/** Skeleton - a set of joints */
typedef struct  
{
	NiteSkeletonJoint joints[NITE_JOINT_COUNT];
	NiteSkeletonState state;
} NiteSkeleton;

/** Snapshot of a specific user */
typedef struct  
{
	NiteUserId id;
	NiteBoundingBox boundingBox;
	NitePoint3f centerOfMass;

	int state;

	NiteSkeleton skeleton;

	NitePoseData poses[NITE_POSE_COUNT];
} NiteUserData;

/** Snapshot of the scene segmentation*/
typedef struct
{
	NiteUserId* pixels;

	int width;
	int height;

	int stride;
} NiteUserMap;

/** 3D Plane */
typedef struct  
{
	NitePoint3f point;
	NitePoint3f normal;
} NitePlane;

/** Output snapshot of the User Tracker algorithm */
typedef struct  
{
	/** Number of users */
	int userCount;
	/** List of users */
	NiteUserData* pUser;

	/** Scene segmentation map */
	NiteUserMap userMap;
	/** The depth frame from which this data was learned */
	OniFrame* pDepthFrame;

	unsigned long long timestamp;
	int frameIndex;

	/** Confidence of the floor plane */
	float floorConfidence;
	/** Floor plane */
	NitePlane floor;
} NiteUserTrackerFrame;

typedef struct  
{
	OniGeneralCallback readyForNextFrame;
} NiteUserTrackerCallbacks;


typedef short int NiteHandId;

/** A snapshot of a specific hand */
typedef struct
{
	NiteHandId id;
	NitePoint3f position;
	int state;
} NiteHandData;

/** A snapshot of a specific gesture */
typedef struct  
{
	NiteGestureType type;
	NitePoint3f currentPosition;
	int state;
} NiteGestureData;

/** Output snapshot of the Hand Tracker algorithm */
typedef struct
{
	/** Number of hands */
	int handCount;
	/** List of hands */
	NiteHandData* pHands;

	/** Number of gestures */
	int gestureCount;
	/** List of gestures */
	NiteGestureData* pGestures;

	/** The depth frame from which this data was learned */
	OniFrame* pDepthFrame;

	unsigned long long timestamp;
	int frameIndex;

} NiteHandTrackerFrame;

typedef struct  
{
	OniGeneralCallback readyForNextFrame;
} NiteHandTrackerCallbacks;

#define _NITE_DECLARE_VERSION(name)																\
/** Holds a NiTEversion number, which consists of four separate numbers in the format: major.minor.maintenance.build*/  \
																								\
typedef struct																					\
{																								\
	/** Major version number, incremented for major API restructuring. */						\
	int major;																					\
	/** Minor version number, incremented when significant new features added. */				\
	int minor;																					\
	/** Maintenance build number, incremented for new releases that primarily 					\
        provide minor bug fixes. */																\
	int maintenance;																			\
	/** Build number. Incremented for each new API build. Generally not shown					\
	on the installer and download site. */														\
	int build;																					\
} name;

_NITE_DECLARE_VERSION(NiteVersion);


typedef struct NiteHandTracker* NiteHandTrackerHandle;

#endif // _NITE_C_TYPES_H_
