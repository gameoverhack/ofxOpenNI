/*
 *  ofxKinectGenerator.cpp
 *  opennisample
 *
 *  Created by Jonas Jongejan on 07/01/11.
 *
 */

#include "ofxDepthGenerator.h"


XnUInt8 PalletIntsR [256] = {0};
XnUInt8 PalletIntsG [256] = {0};
XnUInt8 PalletIntsB [256] = {0};



void CreateRainbowPallet()
{
	unsigned char r, g, b;
	for (int i=1; i<255; i++)
	{
		if (i<=29)
		{
			r = (unsigned char)(129.36-i*4.36);
			g = 0;
			b = (unsigned char)255;
		}
		else if (i<=86)
		{
			r = 0;
			g = (unsigned char)(-133.54+i*4.52);
			b = (unsigned char)255;
		}
		else if (i<=141)
		{
			r = 0;
			g = (unsigned char)255;
			b = (unsigned char)(665.83-i*4.72);
		}
		else if (i<=199)
		{
			r = (unsigned char)(-635.26+i*4.47);
			g = (unsigned char)255;
			b = 0;
		}
		else
		{
			r = (unsigned char)255;
			g = (unsigned char)(1166.81-i*4.57);
			b = 0;
		}
		
		PalletIntsR[i] = r;
		PalletIntsG[i] = g;
		PalletIntsB[i] = b;
	}
}

ofxDepthGenerator::ofxDepthGenerator(){
	CreateRainbowPallet();	
	_depthColoring = 3;
}

bool ofxDepthGenerator::setup(ofxOpenNIContext * context){
	_context = context;
	
	//Create depth generator
	XnStatus nRetVal = g_DepthGenerator.Create(*_context->xnContext());
	if (nRetVal != XN_STATUS_OK){
		printf("Setup Depth Camera failed: %s\n", xnGetStatusString(nRetVal));
		return false;
	} else {
		ofLog(OF_LOG_VERBOSE, "Depth camera inited");
		
		//Set the input to VGA (standard is QVGA wich is not supported on the Kinect)
		XnMapOutputMode mapMode; mapMode.nXRes = XN_VGA_X_RES; mapMode.nYRes = XN_VGA_Y_RES; mapMode.nFPS = 30;
		nRetVal = g_DepthGenerator.SetMapOutputMode(mapMode);
		g_fMaxDepth = g_DepthGenerator.GetDeviceMaxDepth();		
		
		_depthTexture.allocate(mapMode.nXRes, mapMode.nYRes, GL_RGBA);		
		depthPixels = new unsigned char[mapMode.nXRes * mapMode.nYRes * 4];
		memset(depthPixels, 0, mapMode.nXRes * mapMode.nYRes * 4 * sizeof(unsigned char));
		
		
		g_DepthGenerator.StartGenerating();		
		return true;
	}
}

void ofxDepthGenerator::draw(float x, float y, float w, float h){
	generateTexture();
	
	_depthTexture.draw(x, y, w, h);	
}

void ofxDepthGenerator::generateTexture(){
	xn::DepthMetaData pDepthMD;
	g_DepthGenerator.GetMetaData(pDepthMD);	
	const XnDepthPixel* pDepth = pDepthMD.Data();
	XN_ASSERT(pDepth);
	
	if (pDepthMD.FrameID() == 0){
		return;
	}
	
	
	// copy depth into texture-map
	for (XnUInt16 nY = pDepthMD.YOffset(); nY < pDepthMD.YRes() + pDepthMD.YOffset(); nY++){
		unsigned char * pTexture = (unsigned char*)depthPixels + nY * pDepthMD.XRes() * 4 + pDepthMD.XOffset()*4;
		for (XnUInt16 nX = 0; nX < pDepthMD.XRes(); nX++, pDepth++, pTexture+=4){
			XnUInt8 nRed = 0;
			XnUInt8 nGreen = 0;
			XnUInt8 nBlue = 0;
			XnUInt8 nAlpha = 255;
			
			XnUInt16 nColIndex;
			
			
			switch (_depthColoring){
				case 0: //PSYCHEDELIC_SHADES
					nAlpha *= (((XnFloat)(*pDepth % 10) / 20) + 0.5);
				case 1: //PSYCHEDELIC					
					switch ((*pDepth/10) % 10){
						case 0:
							nRed = 255;
							break;
						case 1:
							nGreen = 255;
							break;
						case 2:
							nBlue = 255;
							break;
						case 3:
							nRed = 255;
							nGreen = 255;
							break;
						case 4:
							nGreen = 255;
							nBlue = 255;
							break;
						case 5:
							nRed = 255;
							nBlue = 255;
							break;
						case 6:
							nRed = 255;
							nGreen = 255;
							nBlue = 255;
							break;
						case 7:
							nRed = 127;
							nBlue = 255;
							break;
						case 8:
							nRed = 255;
							nBlue = 127;
							break;
						case 9:
							nRed = 127;
							nGreen = 255;
							break;
					}
					break;
				case 2: //RAINBOW
					nColIndex = (XnUInt16)(((*pDepth) / (g_fMaxDepth / 256)));
					nRed = PalletIntsR[nColIndex];
					nGreen = PalletIntsG[nColIndex];
					nBlue = PalletIntsB[nColIndex];
					break;
				case 3: //CYCLIC_RAINBOW
					nColIndex = (*pDepth % 256);
					nRed = PalletIntsR[nColIndex];
					nGreen = PalletIntsG[nColIndex];
					nBlue = PalletIntsB[nColIndex];
					break;
			}
			
			pTexture[0] = nRed;
			pTexture[1] = nGreen;
			pTexture[2] = nBlue;
			
			if (*pDepth == 0)
				pTexture[3] = 0;
			else
				pTexture[3] = nAlpha;
		}	
	}
	
	_depthTexture.loadData((unsigned char *)depthPixels,pDepthMD.XRes(), pDepthMD.YRes(), GL_RGBA);	
}