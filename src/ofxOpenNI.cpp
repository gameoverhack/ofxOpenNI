/*
 * ofxOpenNI.cpp
 *
 * Copyright 2011-2013 (c) Matthew Gingold [gameover] http://gingold.com.au
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

#include "ofxOpenNI.h"

//--------------------------------------------------------------
ofxOpenNI::ofxOpenNI(){
    stop();
    CreateRainbowPallet();
    _motion_detection_rate = 1000/MOTION_DETECTION_FPS;
    _motion_detection_count = ceil(MOTION_DETECTION_CACHE_LENGTH_MS / (1000/MOTION_DETECTION_FPS));
    _md_threshold_motion = 0.5f;
    _md_threshold_nomotion = 2.0f;
}

//--------------------------------------------------------------
ofxOpenNI::~ofxOpenNI(){
    stop();
}

void ofxOpenNI::setMotionThresholds(float _threshold_motion, float _threshold_nomotion, float _cache_length = 0) {
    ofScopedLock lock(mutex);
    _md_threshold_motion = _threshold_motion;
    _md_threshold_nomotion = _threshold_nomotion;
    if (_cache_length == 0) _cache_length = MOTION_DETECTION_CACHE_LENGTH_MS;
    _motion_detection_count = ceil(MOTION_DETECTION_CACHE_LENGTH_MS / (1000/MOTION_DETECTION_FPS));
}


//--------------------------------------------------------------
bool ofxOpenNI::setup(int _deviceid){
    
    openni::Status rc = OpenNI::initialize();
    if (rc != openni::STATUS_OK) {
        ofLogError() << "Failed to initialize OpenNI:" << OpenNI::getExtendedError();
        bUseDevice = false;
    }
    
    /* Print Device List */
    
    openni::Array<DeviceInfo> deviceInfoList;
    OpenNI::enumerateDevices(&deviceInfoList);
    ofLogNotice() << "***************************************************";
    ofLogNotice() << "* Available Devices                               *";
    ofLogNotice() << "***************************************************";

    for(int i = 0; i < deviceInfoList.getSize(); ++i){
        ofLogNotice() << "Device " << ofToString(i) << " (" << deviceInfoList[i].getName() << "): " << deviceInfoList[i].getUri();
        ofLogNotice() << "       Vendor: " << deviceInfoList[i].getVendor() << deviceInfoList[i].getUsbVendorId() << " / " << ofToString(deviceInfoList[i].getUsbProductId());
        
    }
    ofLogNotice() << "***************************************************";
    
    const char* deviceUri = ANY_DEVICE;
    if (_deviceid >= 0) {
        deviceUri = deviceInfoList[_deviceid].getUri();
    }
    rc = device.open(deviceUri);
    if (rc != openni::STATUS_OK) {
        ofLogError() << "Failed to open device:" << OpenNI::getExtendedError();
        bUseDevice = false;
    }else{
        ofLogNotice() << "Succeeded to open device:" << device.getDeviceInfo().getName();
        device.setDepthColorSyncEnabled(true);
        bUseDevice = true;
    }
    return bUseDevice;
    
}

//--------------------------------------------------------------
bool ofxOpenNI::addDepthStream(){
    
    ofScopedLock lock(mutex);
    
    if(!bUseDevice){
        bUseDepth = false;
        ofLogError() << "Failed to add stream because device is not setup!";
        return bUseDepth;
    }
    
    openni::Status rc = depthStream.create(device, SENSOR_DEPTH);
    
    if(rc != openni::STATUS_OK){
        
        ofLogError() << "Failed to add depth stream";
        bUseDepth = false;
        
    }else{
        
        rc = depthStream.start();
        
        if(rc != openni::STATUS_OK){
            ofLogError() << "Failed to start depth stream";
            depthStream.destroy();
            bUseDepth = false;
        }else{
            ofLogNotice() << "Succeeded to add depth stream";
            allocateDepthBuffers();
            bUseDepth = true;
        }
        
    }
    
    return bUseDepth;
}

//--------------------------------------------------------------
bool ofxOpenNI::addImageStream(){
    
    ofScopedLock lock(mutex);
    
    if(!bUseDevice){
        bUseImage = false;
        ofLogError() << "Failed to add stream because device is not setup!";
        return bUseImage;
    }
    
    openni::Status rc = imageStream.create(device, SENSOR_COLOR);
    
    if(rc != openni::STATUS_OK){
        
        ofLogError() << "Failed to add image stream";
        bUseImage = false;
        
    }else{
        
        rc = imageStream.start();
        
        if(rc != openni::STATUS_OK){
            ofLogError() << "Failed to start image stream";
            imageStream.destroy();
            bUseImage = false;
        }else{
            ofLogNotice() << "Succeeded to add image stream";
            allocateImageBuffers();
            bUseImage = true;
        }
        
    }
    
    return bUseImage;
    
}

//--------------------------------------------------------------
bool ofxOpenNI::addUserTracker(float _smoothing){
    
    ofScopedLock lock(mutex);
    
    if(!bUseDevice){
        bUseUsers = false;
        ofLogError() << "Failed to add stream because device is not setup!";
        return bUseUsers;
    }
    
    if(!bUseNite){
        NiTE::initialize();
        bUseNite = true;
    }
    
    nite::Status rc = userTracker.create(&device);
    
    if(rc != nite::STATUS_OK){
        
        ofLogError() << "Failed to add user tracker";
        bUseUsers = false;
        
    }else{
        
        ofLogNotice() << "Succeded to add user tracker";
        userTracker.setSkeletonSmoothingFactor(_smoothing);
        bUseUsers = true;
    }
    
    return bUseUsers;
    
}

//--------------------------------------------------------------
bool ofxOpenNI::addHandsTracker(){
    
    ofScopedLock lock(mutex);
    
    if(!bUseDevice){
        bUseHands = false;
        ofLogError() << "Failed to add stream because device is not setup!";
        return bUseHands;
    }
    
    if(!bUseNite){
        NiTE::initialize();
        bUseNite = true;
    }
    
    nite::Status rc = handTracker.create(&device);
    
    if(rc != nite::STATUS_OK){
        
        ofLogError() << "Failed to add hand tracker";
        bUseHands = false;
        
    }else{
        
        ofLogNotice() << "Succeeded to add hand tracker";
        
        handTracker.startGestureDetection(GESTURE_WAVE);
        handTracker.startGestureDetection(GESTURE_CLICK);
        
        bUseHands = true;
    }
    
    return bUseHands;
    
}

//--------------------------------------------------------------
void ofxOpenNI::allocateDepthBuffers(){
    depthWidth = depthStream.getVideoMode().getResolutionX();
    depthHeight = depthStream.getVideoMode().getResolutionY();
    depthPixels.allocate(depthWidth, depthHeight, OF_PIXELS_RGBA);
    depthTexture.allocate(depthPixels);
}

//--------------------------------------------------------------
void ofxOpenNI::allocateImageBuffers(){
    imageWidth = imageStream.getVideoMode().getResolutionX();
    imageHeight = imageStream.getVideoMode().getResolutionY();
    imagePixels.allocate(imageWidth, imageHeight, OF_PIXELS_RGB);
    imageTexture.allocate(imagePixels);
}

//--------------------------------------------------------------
void ofxOpenNI::start(){
    if(bUseDevice){
        ofLogNotice() << "Starting ofxOpenNI!";
        startThread();
    }else{
        ofLogError() << "Failed to start! You need to setup your device";
    }
    
}

//--------------------------------------------------------------
void ofxOpenNI::stop(){
    
    if(isThreadRunning()){
        ofLogNotice() << "Stopping ofxOpenNI!";
        stopThread();
    }
    
    if(bUseDevice){
        ofLogNotice() << "Closing openNI device" << device.getDeviceInfo().getName();
        device.close();
    }
    
    depthWidth = depthHeight = 0.0f;
    imageWidth = imageHeight = 0.0f;
    
    bIsDepthFrameNew = bIsImageFrameNew = false;
    
    bUseDevice = false;
    bUseDepth = false;
	bUseImage = false;
	bUseInfra = false;
    bUseUsers = false;
    bUseGesture = false;
    bUseHands = false;
	bUseAudio = false;
	bUseDepthRaw = false;
    bUseRecord = false;
    bUsePlayer = false;
}

//--------------------------------------------------------------
void ofxOpenNI::update(){
    
    if(!bUseDevice) return;
    
    if(isDepthFrameNew()){
       depthTexture.loadData(depthPixels.getPixels(), depthWidth, depthHeight, GL_RGBA);
    }

    if(isImageFrameNew()){
        imageTexture.loadData(imagePixels.getPixels(), imageWidth, imageHeight, GL_RGB);
    }
    
}

//--------------------------------------------------------------
void ofxOpenNI::updateGenerators(){
    
    lock();
    
    if(bUseDepth) depthStream.readFrame(&depthFrame);
    if(bUseImage) imageStream.readFrame(&imageFrame);
    if(bUseUsers) userTracker.readFrame(&userFrame);
    if(bUseHands) handTracker.readFrame(&handFrame);
    
    updateDepthFrame();
    updateImageFrame();
    updateUserFrame();
    updateHandFrame();
    
    unlock();

    ofSleepMillis(30);
    
}

//--------------------------------------------------------------
void ofxOpenNI::updateDepthFrame(){
    if(depthFrame.isValid()){
        const openni::DepthPixel* depth = (const openni::DepthPixel*)depthFrame.getData();
        for (int y = depthFrame.getCropOriginY(); y < depthFrame.getHeight() + depthFrame.getCropOriginY(); y++){
            unsigned char * texture = depthPixels.getPixels() + y * depthFrame.getWidth() * 4 + depthFrame.getCropOriginX() * 4;
            for (int x = 0; x < depthPixels.getWidth(); x++, depth++, texture += 4) {
                ofColor depthColor;
                
                getDepthColor(COLORING_RAINBOW, *depth, depthColor, 10000);
                
                texture[0] = depthColor.r;
                texture[1] = depthColor.g;
                texture[2] = depthColor.b;
                
                if(*depth == 0){
                    texture[3] = 0;
                }else{
                    texture[3] = depthColor.a;
                }
                
            }
        }
        bIsDepthFrameNew = true;
    }
}

//--------------------------------------------------------------
void ofxOpenNI::updateImageFrame(){
    if(imageFrame.isValid()){
        imagePixels.setFromPixels((unsigned char *)imageFrame.getData(), imageFrame.getWidth(), imageFrame.getHeight(), OF_IMAGE_COLOR);
        bIsImageFrameNew = true;
    }
}

//--------------------------------------------------------------
void ofxOpenNI::updateUserFrame(){
    if(userFrame.isValid()){
        const nite::Array<nite::UserData>& users = userFrame.getUsers();

        for(int i = 0; i < users.getSize(); ++i){
            
            
            const UserData& user = users[i];
            
            if(user.isNew()){
                ofLogNotice() << "Found user id: " << user.getId();
                userTracker.startSkeletonTracking(user.getId());
                ofxOpenNIUser u;
                trackedUsers[user.getId()] = u;
                trackedUsers[user.getId()].setUserID(user.getId());
            }

            
            switch (user.getSkeleton().getState()) {
                case nite::SKELETON_TRACKED:
                {
                    //ofLogNotice() << "Skeleton Tracking: " << user.getId();
                    ofxOpenNIUser& u = trackedUsers[user.getId()];
                    u.bIsTracked = true;
                    float timestamp = ofGetElapsedTimeMillis();
                    vector<ofxOpenNIJoint>& joints = u.getJoints();
                    float totalConfidence = 0.0f;
                    for(int i = 0; i < joints.size(); i++){
                        ofxOpenNIJoint& joint = joints[i];
                        
                        u.centerOfMass = toOf(user.getCenterOfMass());
                        
                        nite::Point3f position = user.getSkeleton().getJoint((nite::JointType)i).getPosition();
                        joint.positionReal = toOf(position);
                        ofPoint p;
                        userTracker.convertJointCoordinatesToDepth(position.x, position.y, position.z, &p.x, &p.y);
                        joint.positionProjective = p;
                        joint.positionConfidence = user.getSkeleton().getJoint((nite::JointType)i).getPositionConfidence();
                        totalConfidence += joint.positionConfidence;
                        joint.orientation = toOf(user.getSkeleton().getJoint((nite::JointType)i).getOrientation());
                        joint.orientationConfidence = user.getSkeleton().getJoint((nite::JointType)i).getOrientationConfidence();

                        /* Caching Position, every 100ms */
                        
                        if (joint.activateMotionDetection) {
                         
                            
                            if (timestamp - joint.lastMeasureTimeStamp >= _motion_detection_rate) {

                                
                                joint.lastMeasureTimeStamp = timestamp;
                                
                                float _delta = (joint.pointCache.size()>0?p.distance(joint.pointCache.front().positionProjective):0) / 10;
                                joint.totalDistance += _delta;
                                joint.pointCache.push_front((ofxOpenNIJoint::_queue) {
                                    .distanceMoved = _delta,
                                    .positionProjective = p
                                });
                                

                                /* Limit Length to max 50 Points */
                                if (joint.pointCache.size()>_motion_detection_count) {
                                    
                                   /* if (i==6) {
                                    cout << "Tracking Active Point " << ofToString(i) << ": Distance: " <<   ofToString(joint.motionCache) << " / Speed: " << ofToString(joint.averageSpeed) << " / Current Distance: " << ofToString(joint.motionShortCache) << " / Current Speed: " << ofToString(joint.currentSpeed) << " / Count: " << ofToString(_motion_detection_count) << " / " << ofToString(ceil(_motion_detection_count/5)) << "\n";
                                    }
                                    */
                                    
                                    joint.pointCache.resize(_motion_detection_count);

                                    /* Calculate total and average */

                                    
                                    joint.motionCache       =
                                   // joint.averageSpeed      =
                                   // joint.motionShortCache  =
                                    joint.currentSpeed      = 0.0f;
                                    
                                    int _index = 0;
                                    //int _motion_detection_short_count = ceil(_motion_detection_count/10);
                                    for (std::list<ofxOpenNIJoint::_queue>::iterator _cache = joint.pointCache.begin(); _cache != joint.pointCache.end(); _cache++, _index++) {
                                     //   if (_index < _motion_detection_short_count) {
                                     //       joint.motionShortCache += _cache->distanceMoved;
                                     //   }
                                     //   else {
                                            joint.motionCache += _cache->distanceMoved;
                                     //   }
                                    }
                                    //joint.currentSpeed = joint.motionShortCache / _motion_detection_short_count * _motion_detection_rate;
                                    //joint.averageSpeed = joint.motionCache / (_motion_detection_count-_motion_detection_short_count) * _motion_detection_rate;
                                    joint.currentSpeed = joint.motionCache / _motion_detection_count * _motion_detection_rate;
                                    

                                    /* Motion Detection: Still */
                                    if (joint.currentSpeed < _md_threshold_nomotion)
                                    {
                                        joint.hasMotion = false;
                                        if (joint.detectTime==0.0f) {
                                            joint.detectTime = timestamp;
                                        }
                                    }
                                    /* Motion Detection: Move */
                                    
                                    else if (joint.motionCache > _md_threshold_motion ) {
                                        joint.hasMotion = true;
                                        joint.detectTime = 0.0f;
                                    }
                                    
                                    /* Motion Detection: Move /
                                    
                                    if (joint.motionShortCache > _md_threshold_motion ) {
                                        joint.hasMotion = true;
                                        joint.detectTime = 0.0f;
                                    }
                                    / Motion Detection: Still /
                                    else if (joint.averageSpeed > _md_threshold_nomotion_long && joint.currentSpeed < _md_threshold_nomotion_current)
                                    {
                                        joint.hasMotion = false;
                                        if (joint.detectTime==0.0f) {
                                            joint.detectTime = timestamp;
                                        }
                                    }*/
                                }
                            }
                        }
                    }
                    
                    u.box = user.getBoundingBox();
                    
                    //cout << totalConfidence/joints.size() << endl;
                    if(totalConfidence == 0){
                        u.resetSkeleton();
                    }
                    break;
                }
                case nite::SKELETON_CALIBRATING:
                {
                    ofLogNotice() << "Skeleton Tracking Started: " << user.getId();
                    trackedUsers[user.getId()].resetSkeleton();
                    break;
                }
                case nite::SKELETON_NONE:
                {
                    ofLogNotice() << "Skeleton Tracking Stopped: " << user.getId();
                    trackedUsers[user.getId()].resetSkeleton();
                    break;
                }
                case nite::SKELETON_CALIBRATION_ERROR_NOT_IN_POSE:
                case nite::SKELETON_CALIBRATION_ERROR_HANDS:
                case nite::SKELETON_CALIBRATION_ERROR_LEGS:
                case nite::SKELETON_CALIBRATION_ERROR_HEAD:
                case nite::SKELETON_CALIBRATION_ERROR_TORSO:
                {
                    ofLogNotice() << "Skeleton Calibration Failed: " << user.getId();
                    trackedUsers[user.getId()].resetSkeleton();
                    break;
                }
            }
            
            if(user.isVisible()){
                trackedUsers[user.getId()].bIsVisible = true;
            }else{
                trackedUsers[user.getId()].bIsVisible = false;
            }
            
            if(user.isLost()){
                ofLogNotice() << "Lost user id: " << user.getId();
                trackedUsers.erase(trackedUsers.find(user.getId()));
            }
        }

    }
}

//--------------------------------------------------------------
void ofxOpenNI::updateHandFrame(){
    if(handFrame.isValid()){
        const nite::Array<nite::GestureData>& gestures = handFrame.getGestures();
        for(int i = 0; i < gestures.getSize(); ++i){
            if(gestures[i].isComplete()){
                const Point3f& position = gestures[i].getCurrentPosition();
                ofLogNotice() << "Detected hand gesture: " << gestures[i].getType() << " at " << position.x << ", " << position.y << ", " << position.z;
                HandId newId;
                handTracker.startHandTracking(gestures[i].getCurrentPosition(), &newId);
            }
        }
        
        const nite::Array<nite::HandData>& hands = handFrame.getHands();
        for(int i = 0; i < hands.getSize(); ++i){
            const HandData& hand = hands[i];
            
            if(!hand.isTracking()){
                ofLogNotice() << "Lost hand id: " << hand.getId();
                HandId id = hand.getId();
                trackedHands.erase(trackedHands.find(id));
            }else{
                if(hand.isNew()){
                    ofLogNotice() << "Found hand id: " << hand.getId();
                    ofxOpenNIHand h;
                    trackedHands[hand.getId()] = h;
                    trackedHands[hand.getId()].setID(hand.getId());
                }
                // Add to history
                ofxOpenNIHand& h = trackedHands[hand.getId()];
                const nite::Point3f& position = hand.getPosition();
                ofPoint p;
                handTracker.convertHandCoordinatesToDepth(position.x, position.y, position.z, &p.x, &p.y);
                h.setPositionProjective(p);
                p = toOf(position);
                h.setPositionReal(p);
            }
        }
    }
}

//--------------------------------------------------------------
bool ofxOpenNI::isDepthFrameNew(){
    ofScopedLock lock(mutex);
    return bIsDepthFrameNew;
}

//--------------------------------------------------------------
bool ofxOpenNI::isImageFrameNew(){
    ofScopedLock lock(mutex);
    return bIsImageFrameNew;
}

//--------------------------------------------------------------
void ofxOpenNI::threadedFunction(){
    while (isThreadRunning()) {
        updateGenerators();
    }
}

//--------------------------------------------------------------

map<int, ofxOpenNIUser> ofxOpenNI::skeletonData() {
    ofScopedLock lock(mutex);
    return trackedUsers;
}

//--------------------------------------------------------------

ofTexture & ofxOpenNI::depthData() {
    return depthTexture;
}

//--------------------------------------------------------------

ofTexture & ofxOpenNI::imageData() {
    return imageTexture;
}



//--------------------------------------------------------------
void ofxOpenNI::draw(){
    if(!bUseDevice) return;
    
    ofSetColor(255, 255, 255);
    
    ofPushMatrix();
    if(bUseDepth){
        depthTexture.draw(0, 0, depthWidth, depthHeight);
        ofTranslate(depthWidth, 0);
    }
    if(bUseImage){
        imageTexture.draw(0, 0, imageWidth, imageHeight);
    }
    
    ofPopMatrix();
    
    if(trackedHands.size() > 0){
        for(map<int, ofxOpenNIHand>::iterator it = trackedHands.begin(); it != trackedHands.end(); ++it){
            ofxOpenNIHand& h = it->second;
            h.draw();
        }
    }
    
    if(trackedUsers.size() > 0){
        for(map<int, ofxOpenNIUser>::iterator it = trackedUsers.begin(); it != trackedUsers.end(); ++it){
            ofxOpenNIUser& u = it->second;
            u.draw();
        }
    }
    
    ofSetColor(255, 255, 255);
}
