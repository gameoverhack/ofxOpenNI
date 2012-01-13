/*
 * ofxOpenNIContext.h
 *
 * Copyright 2011 (c) Matthew Gingold [gameover] http://gingold.com.au
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

#ifndef	_H_OFXOPENNICONTEXT
#define _H_OFXOPENNICONTEXT

#include <XnOpenNI.h>
#include <XnCodecIDs.h>
#include <XnCppWrapper.h>
#include <XnLog.h>
#include <XnTypes.h>
#include "ofxOpenNIUtils.h"
#include "ofLog.h"
#include "ofConstants.h"
#include "ofPixels.h"
#include "ofTexture.h"
#include "ofThread.h"

static ofMutex openNIMutex;

using namespace xn;

class ofxOpenNI;
class ofxOpenNIContext : public ofThread {
	
public:
	
	ofxOpenNIContext();
	~ofxOpenNIContext();
	
    bool initContext();
	void setLogLevel(XnLogSeverity logLevel);
	
	int getNumDevices();
	bool getIsContextReady();
    
    bool getPaused();
    void setPaused(bool b);
    void togglePaused();
    
    xn::Context& getContext();
    
	static string LOG_NAME;
    
protected:
	
	void threadedFunction();
    
private:
    
    friend class ofxOpenNI;
    
	bool addLicence(string sVendor, string sKey);
	
	xn::Device& getDevice(int deviceID);
	xn::DepthGenerator& getDepthGenerator(int deviceID);
	xn::ImageGenerator& getImageGenerator(int deviceID);
	xn::IRGenerator& getIRGenerator(int deviceID);
	xn::AudioGenerator& getAudioGenerator(int deviceID);
    xn::UserGenerator& getUserGenerator(int deviceID);
	xn::Player& getPlayer(int deviceID);
    
    bool addDeviceNode(int deviceID);
	bool addDepthNode(int deviceID);
    bool addImageNode(int deviceID);
    bool addInfraNode(int deviceID);
    bool addUserNode(int deviceID);
    bool addAudioNode(int deviceID);
    
    bool stopDepthNode(int deviceID);
    bool stopImageNode(int deviceID);
    bool stopInfraNode(int deviceID);
    bool stopUserNode(int deviceID);
    bool stopAudioNode(int deviceID);
    
    int initDevices();
	int enumerateAndCreateXnNode(XnProductionNodeType type, ProductionNode *node, int deviceID = -1);
    int enumerateXnNode(XnProductionNodeType type, NodeInfoList & list);
    bool createXnNode(XnProductionNodeType type, ProductionNode & node, int nodeIndex);
	void logErrors(xn::EnumerationErrors & errors);

    bool bPaused;
	bool bIsContextReady;
	
	// generators/nodes
	xn::Context g_Context;
	vector<xn::Device> g_Device;
	vector<xn::DepthGenerator> g_Depth;
	vector<xn::ImageGenerator> g_Image;
	vector<xn::IRGenerator> g_IR;
	vector<xn::UserGenerator> g_User;
	vector<xn::AudioGenerator> g_Audio;
	vector<xn::Player> g_Player;
    
};

// Singleton class and ofxOpenNIContext static singleton type defintion
#ifndef _H_SINGLETON
#define _H_SINGLETON
#include "assert.h"
#include <cstdlib>
#include "ofTypes.h"
template <class T>
class Singleton {
public:
	static ofPtr<T> Instance() {
		if(!m_pInstance) m_pInstance = ofPtr<T>(new T);
		assert(m_pInstance !=NULL);
		return m_pInstance;
	}
protected:
	Singleton(){m_pInstance = NULL;};
	~Singleton();
private:
	Singleton(Singleton const&);
	Singleton& operator=(Singleton const&);
	static ofPtr<T> m_pInstance;
};
template <class T> ofPtr<T> Singleton<T>::m_pInstance; // = NULL;
#endif

typedef Singleton<ofxOpenNIContext> ofxOpenNIContextSingleton;
static ofPtr<ofxOpenNIContext> openNIContext = ofxOpenNIContextSingleton::Instance();
#endif

// Singleton class and ofxOpenNIContext static singleton type defintion
//#ifndef _H_SINGLETON
//#define _H_SINGLETON
//#include "assert.h"
//#include <cstdlib>
////#include "ofTypes.h"
//template <class T>
//class Singleton {
//public:
//	static T* Instance() {
//		if(!m_pInstance) m_pInstance = new T;
//		assert(m_pInstance !=NULL);
//		return m_pInstance;
//	}
//protected:
//	Singleton();
//	~Singleton();
//private:
//	Singleton(Singleton const&);
//	Singleton& operator=(Singleton const&);
//	static T* m_pInstance;
//};
//template <class T> T* Singleton<T>::m_pInstance = NULL;
//#endif
//
//typedef Singleton<ofxOpenNIContext> ofxOpenNIContextSingleton;
//static ofxOpenNIContext * openNIContext = ofxOpenNIContextSingleton::Instance();
//#endif
