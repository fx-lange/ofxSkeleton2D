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

	ofxSLimb(ofPoint & p1, ofPoint & p2);

	void copy(ofxSLimb * other);

	void clear();

	ofPoint & first(){
		return joints.front();
	}

	ofPoint & last(){
		return joints.back();
	}

	ofPoint * getJoint(int idx);

	ofPoint * getLimbStart();
	ofPoint * getLimbEnd();

	void join(ofPoint & jointP, ofPoint & newEndP, bool bInsertFront);

	float getLengthSquared();
	float getDirectLength(){
		return getLimbStart()->distance(*getLimbEnd());
	}

	void draw(float x=0, float y=0);
};

#endif /* OFXSLIMB_H_ */
