/*
 * ofxOpenNITypes.h
 *
 * Copyright 2011-2013 (c) Matthew Gingold [gameover] http://gingold.coau
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

#ifndef	_H_OFXOPENNITYPES
#define _H_OFXOPENNITYPES

#include "ofMain.h"
#include "ofxOpenNIUtils.h"

static int NUMBER_OF_JOINTS = 16;

class ofxOpenNIJoint {
    
public:
    
    ofxOpenNIJoint(){
        
    }
    
    ~ofxOpenNIJoint(){
        
    }
    
    nite::JointType jointType;
    
    ofPoint positionReal;
    ofPoint positionProjective;
    float positionConfidence;
    
    ofQuaternion orientation;
    float orientationConfidence;
};

class ofxOpenNIUser {
    
public:
    
    ofxOpenNIUser(){
        userID = -1;
        bIsTracked = bIsVisible = false;
        joints.resize(NUMBER_OF_JOINTS);
        resetSkeleton();
    }
    
    ~ofxOpenNIUser(){
        joints.clear();
    }
    
    void setUserID(int id){
        userID = id;
    }
    
    int getUserID(){
        return userID;
    }
    
    void resetSkeleton(){
        for(int i = 0; i < joints.size(); i++){
            joints[i].jointType = (nite::JointType)i;
            joints[i].positionReal = ofPoint(0,0,0);
            joints[i].positionConfidence = 0.0f;
            joints[i].positionProjective = ofPoint(0,0,0);
            joints[i].orientation = ofQuaternion(0,0,0,0);
            joints[i].orientationConfidence = 0.0f;
        }
        bIsTracked = false;
    }
    
    bool isTracking(){
        return bIsTracked;
    }
    
    bool isVisible(){
        return bIsVisible;
    }
    
    vector<ofxOpenNIJoint>& getJoints(){
        return joints;
    }
    
    void draw(){
        ofPushMatrix();
        ofFill();
        for(int i = 0; i < joints.size(); i++){
            ofxOpenNIJoint& j = joints[i];
            if(j.positionConfidence > 0.5){
                ofSetColor(0, 255, 0);
            }else{
                ofSetColor(255, 0, 0);
            }
            ofCircle(j.positionProjective.x, j.positionProjective.y, 5);
        }
        ofNoFill();
        ofPopMatrix();
    }
    
protected:
    
    friend class ofxOpenNI;
    
    vector<ofxOpenNIJoint> joints;
    int userID;
    
    bool bIsTracked;
    bool bIsVisible;
    
};

class ofxOpenNIHand {

public:

    ofxOpenNIHand(){
        bUseHistory = true;
        handID = -1;
        historyIndex = 0;
        historyLength = 20;
        history.setMode(OF_PRIMITIVE_LINE_STRIP);
    }
    
    ~ofxOpenNIHand(){
        history.clear();
    }
    
    void setID(int id){
        handID = id;
    }
    
    int getHandID(){
        return handID;
    }
    
    void setPositionProjective(ofPoint& p){
        positionProjective = p;
        if(bUseHistory){
            if(history.getNumVertices() < historyLength){
                history.addVertex(p);
            }else{
                if(historyIndex == historyLength) historyIndex = 0;
                history.setVertex(historyIndex, p);
                historyIndex++;
            }
        }
    }
    
    void setPositionReal(ofPoint& p){
        positionReal = p;
    }
    
    ofPoint& getPositionReal(){
        return positionReal;
    }
    
    ofPoint& getPositionProjective(){
        return positionProjective;
    }
    
    void draw(){
        ofPushMatrix();
        ofFill();
        ofSetColor(255, 255, 0);
        ofCircle(positionProjective.x, positionProjective.y, 5);
        ofNoFill();
        if(bUseHistory) history.draw();
        ofPopMatrix();
    }
    
protected:
    
    int handID;
    
    ofPoint positionReal;
    ofPoint positionProjective;
    ofMesh history;
    
    bool bUseHistory;
    
    int historyIndex;
    int historyLength;
    
};

#endif