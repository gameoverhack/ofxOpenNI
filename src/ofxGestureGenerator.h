/*
 * ofxGestureGenerator.h
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

#ifndef _H_OFXGESTUREGENERATOR
#define _H_OFXGESTUREGENERATOR

#include "ofxOpenNIContext.h"
#include "ofxDepthGenerator.h"
#include "ofEvents.h"

enum {
	GESTURE_RECOGNIZED,
	GESTURE_PROGRESS
};

typedef struct {
	string	gesture_name;
	int		gesture_type;
	float	gesture_progress;
	ofPoint	gesture_position;
	int		gesture_timestamp;
} gesture;

class ofxGestureGenerator {
	
public:
	
	ofxGestureGenerator();
	~ofxGestureGenerator();
	
	bool					setup(ofxOpenNIContext* pContext);

	bool					addGesture(string gesture_name);
	bool					removeGesture(string gesture_name);
	
	void					setMinTimeBetweenGestures(int time);
	int						getMinTimeBetweenGestures();
	
	xn::GestureGenerator&	getXnGestureGenerator();
	
	ofEvent<gesture>				gestureRecognized;
	ofEvent<gesture>				gestureProgress;
	
	// PRIVATE CALLBACK
	
	gesture		*				getLastGesture();
	
	void						setUseProgress(bool b);
	bool						getUseProgress();
	
	void						setVerbose(bool b);	
	
private:
	
	ofxOpenNIContext*		context;
	xn::GestureGenerator	gesture_generator;
	xn::DepthGenerator		depth_generator;
	
	gesture					last_gesture;
	bool					b_use_progress;
	int						min_time;
	
	bool					b_verbose;

	ofxGestureGenerator(ofxGestureGenerator const& other);
	ofxGestureGenerator & operator = (const ofxGestureGenerator&);
	
};


#endif