/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_HAND_POINT_CONTEXT_H_
#define _XNV_HAND_POINT_CONTEXT_H_

#include "XnVNiteDefs.h"

/**
* A XnVHandPointContext describes a single hand point
*/
typedef struct XNV_NITE_API XnVHandPointContext
{
	/**
	* The current position of the hand
	*/
	XnPoint3D ptPosition;
	/**
	* A persistent ID for the hand, allowing tracking
	*/
	XnUInt32 nID;
	/**
	* The user to which this hand belongs
	*/
	XnUInt32 nUserID;
	/**
	* The time in which this hand was in this position
	*/
	XnFloat fTime;
	/**
	* The probability the point is actually there
	* 1 - Fresh update. Lower - no update (or delete) for some time
	*/
	XnFloat fConfidence;
} XnVHandPointContext;

#endif
