/*
 * ofxSLimb.h
 *
 *  Created on: Jun 18, 2012
 *      Author: spta
 */

#ifndef OFXSLIMB_H_
#define OFXSLIMB_H_

#include "ofMain.h"

class SLimb{
public:
	vector<ofPoint> joints;
	SLimb(){	}
	ofColor color;
	bool bReverseOrder;//TODO könnte diesen boolean durch einen entsprechend vorbereiteten revereseIterator ersetzen
	float startAngle;

	SLimb(ofPoint & p1, ofPoint & p2){
		joints.clear();
		joints.push_back(p1);
		joints.push_back(p2);
		color.set(255,255,255,100);
		startAngle = -1.f;
	}

	ofPoint & first(){
		return joints.front();
	}

	ofPoint & last(){
		return joints.back();
	}


	ofPoint & getLimbStart(){
		if(bReverseOrder)
			return joints.back();
		return joints.front();
	}

	ofPoint & getLimbEnd(){
		if(bReverseOrder)
			return joints.front();
		return joints.back();
	}

	void join(ofPoint & jointP, ofPoint & newEndP, bool bInsertFront){
		if(bInsertFront){
			joints.front() = jointP;//TODO Schnittpunkt aus beiden statt Ersetzen
			joints.insert(joints.begin(),newEndP);
		}else{
			joints.back() = jointP;//TODO Schnittpunkt aus beiden statt Ersetzen
			joints.push_back(newEndP);
		}

	}

	void draw(float x=0, float y=0){
		ofPushMatrix();
		ofPushStyle();
		ofTranslate(x,y,0);
		ofSetColor(color);
		ofFill();
		ofPoint & p1 = first();
		ofPoint & p2 = last();
		ofEllipse(p1.x,p1.y,20,20);
		ofEllipse(p2.x,p2.y,20,20);
		ofLine(p1.x,p1.y,p2.x,p2.y);

		ofPopMatrix();
		ofPopStyle();
	}

};

#endif /* OFXSLIMB_H_ */
