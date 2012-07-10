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

	bool isLeftOf(ofxSLimb & other){
		ofPoint & o1 = other.first();
		ofPoint & o2 = other.last();
		float detA1 = first().x * last().y + o1.x * first().y + last().x+ o1.y
				- first().x * o1.y - last().x * first().y - o1.x*last().y;
		float detA2 = first().x * last().y + o2.x * first().y + last().x+ o2.y
				- first().x * o2.y - last().x * first().y - o2.x*last().y;

		if(detA1 >= 0 && detA2 >= 0){
			return -1;
		}else if(detA1 <= 0 && detA2 <= 0){
			return 1;
		}else{
			return 0;
		}
	}

	float getLengthSquared();
	float getDirectLength(){
		return getLimbStart()->distance(*getLimbEnd());
	}

	void draw(float x=0, float y=0);
};

#endif /* OFXSLIMB_H_ */
