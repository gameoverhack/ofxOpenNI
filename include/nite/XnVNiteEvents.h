/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/

#ifndef _XNV_NITE_EVENTS_H_
#define _XNV_NITE_EVENTS_H_

#include <XnOpenNI.h>
#include <XnEvent.h>
#include <XnHash.h>
#include "XnVNiteDefs.h"

XN_DECLARE_DEFAULT_HASH_DECL(XNV_NITE_API, XnUInt32, XnUInt32, XnVIntHash);

XN_DECLARE_LIST_DECL(XNV_NITE_API, XnUInt32, XnVIntList);

XN_DECLARE_EVENT_0ARG(XnVEvent, XnVEventIfc);

XN_DECLARE_EVENT_1ARG(XnVIntSpecificEvent, XnVIntEvent, XnInt32, nValue);
XN_DECLARE_EVENT_1ARG(XnVUintSpecificEvent, XnVUintEvent, XnUInt32, nValue);
XN_DECLARE_EVENT_1ARG(XnVFloatSpecificEvent, XnVFloatEvent, XnFloat, fValue);
XN_DECLARE_EVENT_1ARG(XnVDirectionSpecificEvent, XnVDirectionEvent, XnVDirection, eValue);

XN_DECLARE_EVENT_2ARG(XnVFloatFloatSpecificEvent, XnVFloatFloatEvent, XnFloat, fVal1, XnFloat, fVal2);
XN_DECLARE_EVENT_2ARG(XnVIntIntSpecificEvent, XnVIntIntEvent, XnInt32, nVal1, XnInt32, nVal2);
XN_DECLARE_EVENT_2ARG(XnVUintPointSpecificEvent, XnVUintPointEvent, XnUInt32, nValue, const XnPoint3D&, ptPos);
XN_DECLARE_EVENT_2ARG(XnVIntDirectionSpecificEvent, XnVIntDirectionEvent, XnInt32, nValue, XnVDirection, eValue);

#endif // _XNV_NITE_EVENTS_H_