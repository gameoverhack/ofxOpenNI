/*
 * ofxOpenNIMacros.h
 *
 * Copyright 2011 (c) Matthew Gingold http://gingold.com.au
 * Originally forked from a project by roxlu http://www.roxlu.com/ 
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#define CHECK_RC(rc, what)											\
if (rc != XN_STATUS_OK)												\
{																	\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));			\
} else {															\
	printf("%s succeed: %s\n", what, xnGetStatusString(rc));		\
}

#define BOOL_RC(rc, what) 											\
if (rc != XN_STATUS_OK)												\
{																	\
	printf("%s failed: %s\n", what, xnGetStatusString(rc));			\
	return false;													\
}																	\
printf("%s succeed: %s\n", what, xnGetStatusString(rc));			\
return true;														\


#define SHOW_RC(rc, what)											\
	printf("%s status: %s\n", what, xnGetStatusString(rc));


#define CHECK_RC_ERR(rc, what, error)								\
{																	\
if (rc == XN_STATUS_NO_NODE_PRESENT)								\
{																	\
	XnChar strError[1024];											\
	errors.ToString(strError, 1024);								\
	printf("%s\n", strError);										\
}																	\
CHECK_RC(rc, what)													\
}
