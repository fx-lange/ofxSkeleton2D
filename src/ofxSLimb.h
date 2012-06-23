#ifndef OFXSLIMB_H_
#define OFXSLIMB_H_

#include "ofMain.h"

class ofxSLimb{
public:
	vector<ofPoint> joints;
	ofColor color;
	bool bReverseOrder;//TODO replace boolean with revereseIterator
	float startAngle;

	bool bFound;

	ofxSLimb(){
		bFound = false;
	}

	ofxSLimb(ofPoint & p1, ofPoint & p2){
		joints.clear();
		joints.push_back(p1);
		joints.push_back(p2);
		color.set(255,255,255,50);
		startAngle = -1.f;
		bFound = true;
	}

	void copy(ofxSLimb * other){
		bReverseOrder = other->bReverseOrder;
		color = other->color;
		joints.insert(joints.begin(),other->joints.begin(),other->joints.end());
		bFound = true;
	}

	void clear(){
		bFound = false;
		joints.clear();
	}

	ofPoint & first(){
		return joints.front();
	}

	ofPoint & last(){
		return joints.back();
	}


	ofPoint * getLimbStart(){
		if(!bFound)
			return NULL;

		if(bReverseOrder)
			return &joints.back();
		return &joints.front();
	}

	ofPoint * getLimbEnd() {
		if(!bFound)
			return NULL;

		if(bReverseOrder)
			return &joints.front();
		return &joints.back();
	}

	void join(ofPoint & jointP, ofPoint & newEndP, bool bInsertFront){
		if(bInsertFront){
			joints.front() = jointP;//TODO insert intersection (or median)
			joints.insert(joints.begin(),newEndP);
		}else{
			joints.back() = jointP;//TODO insert intersection (or median)
			joints.push_back(newEndP);
		}

	}

	float getLengthSquared(){
		float result = 0;
		for(int i=0;i<joints.size()-1;++i){
			ofPoint & p = joints[i];
			ofPoint & q = joints[i+1];
			result += p.squareDistance(q);
		}
		return result;
	}

	void draw(float x=0, float y=0){
		ofPushMatrix();
		ofPushStyle();
		ofTranslate(x,y,0);
		ofSetColor(color);
		ofFill();
		ofPoint & p1 = first();
		ofPoint & p2 = last();
		ofEllipse(p1.x,p1.y,10,10);
		ofEllipse(p2.x,p2.y,10,10);
		ofLine(p1.x,p1.y,p2.x,p2.y);

		for(int i=0;i<joints.size()-1;++i){
			ofPoint & p = joints[i];
			ofPoint & q = joints[i+1];
			int dist = round(p.distance(q));
			ofVec3f middle = (p+q)/2.f;
			ofSetColor(255,255,255);
			ofDrawBitmapString(ofToString(dist),middle);
		}

		ofPopMatrix();
		ofPopStyle();
	}

};

#endif /* OFXSLIMB_H_ */
