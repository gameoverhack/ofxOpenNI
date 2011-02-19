#pragma once

#include <string>
#include <vector>
#include <sstream>
#include "ofMain.h"

using namespace std;

struct NILicense {
	NILicense(string sVendor, string sKey):vendor(vendor),key(sKey){};
	string vendor;
	string key;
	
	string getXML() {
	}
};

struct NIMask {
	NIMask(string sName, bool bOn):name(sName),on(bOn) {}
	string name;
	bool on;
	
	string getXML() {
	}
};

struct NIAttribute {
	NIAttribute(string sName, string sValue):name(sName),value(sValue){}
	string name;
	string value;
	
	string getXML() {
		ostringstream oss;
		oss << name << "=\"" << value << "\" ";
		return oss.str();
	}
};

struct NINode {
	NINode(string sName, string sType, string sTagName = "Node"):name(sName),type(sType),tagname(sTagName){}
	vector<NIAttribute> attributes;
	string type;
	string name;
	string tagname;
	
	NINode& addAttribute(string sName, string sValue) {
		NIAttribute attribute(sName, sValue);
		attributes.push_back(attribute);
		return *this;
	}
	
	string getXML() {
		return "";
	}
};

struct NITag {
	string name;
	vector<NIAttribute> attributes;
	
	NITag(){}
	NITag(string sName):name(sName){};
	
	NITag& addAttribute(string sName, string sValue) {
		NIAttribute attribute(sName, sValue);
		attributes.push_back(attribute);
		return *this;
	}
	
	NITag& addAttribute(string sName, int nValue) {
		ostringstream oss;
		oss << nValue;
		return addAttribute(sName, oss.str());
	}
	
	string getXML(string sChildXML = "") {
		ostringstream oss;
		
		// Node atributes.
		vector<NIAttribute>::iterator it_att = attributes.begin();
		oss << "<" << name;
		while(it_att != attributes.end()) {
			oss << " "  << (*it_att).getXML();
			++it_att;
		}
		oss << ">\n";
		oss << sChildXML;
		oss << "</" << name <<">\n";
		return oss.str();
	}
};

class ofxOpenNIXML {
public:
	vector<NITag> nodes;
	vector<NITag> licenses;
	vector<NITag> masks;
	NITag log;
	NITag log_level;
	
	ofxOpenNIXML() {
		log.name = "Log";
		log_level.name = "LogLevel";
	}
	
	NITag& setLogging(bool bWriteToConsole, bool bWriteToFile, int nLogLevel) {
		//<Log writeToConsole="true" writeToFile="true">
		log.addAttribute("writeToConsole", (bWriteToConsole) ? "true" : "false");
		log.addAttribute("writeToFile", (bWriteToFile) ? "true" : "false");
	
		log_level.addAttribute("value", nLogLevel);
	}
			
	NITag& addLoggingMask(string sMask, bool bOn) {
		NITag tag("mask");
		tag.addAttribute("on", (bOn) ? "true" : "false");
		masks.push_back(tag);
		return masks.back();
	}
	
	NITag& addLicense(string sVendor, string sKey) {
		NITag tag("License");
		tag.addAttribute("vendor", sVendor);
		tag.addAttribute("key", sKey);
		licenses.push_back(tag);
		return licenses.back();
	}
	
	NITag& addRecording(string sFile) {
		NITag tag("Recording");
		tag.addAttribute("file",ofToDataPath(sFile,true));
		nodes.push_back(tag);
		return nodes.back();
	}
	
	
	NITag& addUserNode() {
		NITag user("Node");
		user.addAttribute("type", "User");
		nodes.push_back(user);
		return nodes.back();
	}
	
	string tagsToXML(vector<NITag>& rTags, string sWrapper = "") {
		
		vector<NITag>::iterator it = rTags.begin();
		ostringstream oss;
		if(sWrapper != "") {
			oss << "<" << sWrapper << ">\n";
		}
		
		while(it != rTags.end()) {
			oss << (*it).getXML() << "\n";
			++it;
		}
		
		if(sWrapper != "") {
			oss << "</" << sWrapper << ">\n";
		}
		return oss.str();
	}
	
	string getXML() {
		stringstream oss;
		oss << "<OpenNI>\n";
		
		// licenses
		oss << tagsToXML(licenses, "Licenses") << std::endl;
		
		
		// logging.
		string log_xml = tagsToXML(masks, "Masks");
		log_xml += log_level.getXML();
		oss << log.getXML(log_xml);
		oss << tagsToXML(nodes, "ProductionNodes") << std::endl;
		
		
		oss << "</OpenNI>\n";
		
		return oss.str();
	}
};
/*
 /*
 <OpenNI>
 <Licenses>
	<License vendor="PrimeSense" key="0KOIk2JeIBYClPWVnMoRKn5cdY4="/>
 </Licenses>

 <Log writeToConsole="true" writeToFile="true">
	<!-- 0 - Verbose, 1 - Info, 2 - Warning, 3 - Error (default) -->
	<LogLevel value="2"/>
	<Masks>
	<Mask name="ALL" on="true"/>
 </Masks>
 <Dumps>
 </Dumps>
 </Log>
 <ProductionNodes>
 <Recording file="/Users/diederick/Documents/programming/c++/of62/apps/diederick/001_ofxtweakbar/bin/data/roxlu_user.oni" />
 <!-- ofxDepthGenerator -->
 <!--
 <Node type="Depth" name="Depth1">
 <Configuration>
 <MapOutputMode xRes="640" yRes="480" FPS="30"/>
 <Mirror on="true"/>
 </Configuration>
 </Node>
 -->
 <!-- ofxUserGenerator -->
 <Node type="User" />
 </ProductionNodes>
 </OpenNI>
 */

