/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef _XNV_NITE_STATUS_H_
#define _XNV_NITE_STATUS_H_

#include <XnCommon.h>

XN_PS_STATUS_MESSAGE_MAP_START(XN_ERROR_GROUP_EE_NITE)
XN_STATUS_MESSAGE(XN_STATUS_NITE_OUT_OF_RANGE, "Out of range")
XN_STATUS_MESSAGE(XN_STATUS_NITE_NOT_ENOUGH_TIME, "Not enough time for calculation")
XN_STATUS_MESSAGE(XN_STATUS_NITE_COOLDOWN, "Inside cooldown period")
XN_STATUS_MESSAGE(XN_STATUS_NITE_UNEXPECTED_DIRECTION, "Unexpected direction")
XN_STATUS_MESSAGE(XN_STATUS_NITE_NO_SUCH_POINT, "No such point")
XN_STATUS_MESSAGE(XN_STATUS_NITE_CANT_REMOVE_POINT, "Can't remove points")
XN_STATUS_MESSAGE(XN_STATUS_NITE_SHOULDNT_REMOVE_POINT, "Shouldn't remove points")
XN_STATUS_MESSAGE(XN_STATUS_NITE_ILLEGAL_INPUT, "Input is illegal")
XN_STATUS_MESSAGE(XN_STATUS_NITE_UNSUPPORTED, "Unsupported NITE feature")
XN_STATUS_MESSAGE(XN_STATUS_NITE_DEPTH_UNAVAILABLE, "No depth available!")

XN_STATUS_MESSAGE(XN_STATUS_NITE_NO_TRACKER, "No hand generator found. One is needed for session manager.")
XN_STATUS_MESSAGE(XN_STATUS_NITE_NO_GESTURE, "No gesture generator found. One is needed for session manager.")
XN_STATUS_MESSAGE(XN_STATUS_NITE_NO_MATCHING_GESTURE, "No gesture generator was found that supports all required gestures.")

XN_STATUS_MESSAGE(XN_STATUS_NITE_UNEXPECTED_TYPE, "Object is of not of expected type")
XN_PS_STATUS_MESSAGE_MAP_END(XN_ERROR_GROUP_EE_NITE)

#endif