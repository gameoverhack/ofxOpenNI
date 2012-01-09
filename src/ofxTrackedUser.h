/*
 * ofxTrackedUser.h
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

#ifndef _H_OFXTRACKEDUSER
#define _H_OFXTRACKEDUSER

#include <XnOpenNI.h>
#include <XnCppWrapper.h>
#include "ofxOpenNIContext.h"
#include "ofMain.h"

struct ofxLimb {
	ofxLimb(XnSkeletonJoint nStartJoint, XnSkeletonJoint nEndJoint)
	:start_joint(nStartJoint)
	,end_joint(nEndJoint)
	,found(false)
	{
		position[0].X = position[1].X = 0;
		position[0].Y = position[1].Y = 0;
		position[0].Z = position[1].Z = 0;
	}

	ofxLimb(){};
	~ofxLimb(){};

	XnSkeletonJoint start_joint;
	XnSkeletonJoint end_joint;
	XnPoint3D position[2];
	bool found;

	void debugDraw() {
		if(!found)
			return;
		glPushMatrix();
		glLineWidth(5);
		glColor3f(1,0,0);
		glBegin(GL_LINES);
		glVertex2i(position[0].X, position[0].Y);
		glVertex2i(position[1].X, position[1].Y);
		glEnd();
		glPopMatrix();
	}

};

class ofxOpenNIContext;
class ofxTrackedUser {
public:

	void debugDraw(const float wScale=1.0f, const float hScale=1.0f);

	ofxLimb neck;

	// left arm + shoulder
	ofxLimb left_shoulder;
	ofxLimb left_upper_arm;
	ofxLimb left_lower_arm;

	// right arm + shoulder
	ofxLimb right_shoulder;
	ofxLimb right_upper_arm;
	ofxLimb right_lower_arm;

	// torso
	ofxLimb left_upper_torso;
	ofxLimb right_upper_torso;

	// left lower torso + leg
	ofxLimb left_lower_torso;
	ofxLimb left_upper_leg;
	ofxLimb left_lower_leg;

	// right lower torso + leg
	ofxLimb right_lower_torso;
	ofxLimb right_upper_leg;
	ofxLimb right_lower_leg;

	ofxLimb hip;
	XnUserID id;

    bool skeletonTracking, skeletonCalibrating, skeletonCalibrated;
    XnPoint3D	center;
    
private:

	ofxTrackedUser(ofxOpenNIContext* pContext);

	void updateBonePositions();
	void updateLimb(ofxLimb& rLimb);

	ofxOpenNIContext*		context;
	xn::UserGenerator		user_generator;
	xn::DepthGenerator		depth_generator;

	friend class ofxUserGenerator;
};

#endif
