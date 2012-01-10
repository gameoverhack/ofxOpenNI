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
	Singleton();
	~Singleton();
private:
	Singleton(Singleton const&);
	Singleton& operator=(Singleton const&);
	static ofPtr<T> m_pInstance;
};
template <class T> ofPtr<T> Singleton<T>::m_pInstance=NULL;
#endif
//typedef Singleton<ofxOpenNIContext> ofxOpenNIContextSingleton;
//static ofPtr<ofxOpenNIContext> openNIContext = ofPtr<ofxOpenNIContext>(ofxOpenNIContextSingleton::Instance());
#ifndef	_H_OFXOPENNICONTEXT
#define _H_OFXOPENNICONTEXT

#define MAX_DEVICES 2

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

using namespace xn;

class ofxOpenNIContext : public ofThread {
	
public:
	
	ofxOpenNIContext();
	~ofxOpenNIContext();
	
	bool initContext();
	bool addLicence(string sVendor, string sKey);
	void setLogLevel(XnLogSeverity logLevel);
	
	int getNumDevices();
	
	bool getIsContextReady();
	xn::Context& getContext();
	
	xn::Device& getDevice(int deviceID = 0);
	xn::DepthGenerator& getDepthGenerator(int deviceID = 0);
	xn::ImageGenerator& getImageGenerator(int deviceID = 0);
	xn::IRGenerator& getIRGenerator(int deviceID = 0);
	xn::AudioGenerator& getAudioGenerator(int deviceID = 0);
	xn::Player& getPlayer(int deviceID = 0);
	
	static string LOG_NAME;
	
protected:
	
	void threadedFunction();
	
private:
	
	int enumerateAndCreateXnNode(XnProductionNodeType type, ProductionNode *node);
	void logErrors(xn::EnumerationErrors & errors);
	int numDevices;
	bool bIsContextReady;
	
	// generators/nodes
	xn::Context g_Context;

	xn::Device g_Device[MAX_DEVICES];
	xn::DepthGenerator g_Depth[MAX_DEVICES];
	xn::ImageGenerator g_Image[MAX_DEVICES];
	xn::IRGenerator g_IR[MAX_DEVICES];
	xn::UserGenerator g_User[MAX_DEVICES];
	xn::AudioGenerator g_Audio[MAX_DEVICES];
	xn::Player g_Player[MAX_DEVICES];
	
	xn::ProductionNode *g_pPrimary;
	
	// TODO: use vectors -> although makes enumerateAndCreateXnNodes a little different/harder maybe
	//vector<xn::Device> g_Device;
	//vector<xn::DepthGenerator> g_Depth;
	//vector<xn::ImageGenerator> g_Image;
	//vector<xn::IRGenerator> g_IR;
	//vector<xn::UserGenerator> g_User;
	//vector<xn::AudioGenerator> g_Audio;
	//vector<xn::Player> g_Player;
	
};
typedef Singleton<ofxOpenNIContext> ofxOpenNIContextSingleton;
static ofPtr<ofxOpenNIContext> openNIContext = ofxOpenNIContextSingleton::Instance();
//static ofPtr<ofxOpenNIContext> openNIContext = ofPtr<ofxOpenNIContext>(new ofxOpenNIContext()); // not sure this is the right way to do this???
#endif