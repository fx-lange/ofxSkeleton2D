#ifndef OFXSJOINT_H_
#define OFXSJOINT_H_

#include "ofxGui.h"

#define JOINT_BUFFER_SIZE 3 //TODO dynamic!!

class ofxSJoint : public ofPoint{
public:
	bool bFound;
	//REVISIT each bFound = false should be tracked!

	ofxSJoint(){
		clear();
	}

	ofxSJoint(const ofPoint & p){//TODO REVISIT is used?
		x = p.x;
		y = p.y;
		z = p.z;
		clear();
		positionBuffer.push_back(p);
		bFound = true;
	}

	void clear(){
		pointsAdded = idx = 0;
		bFound = false;
		N = JOINT_BUFFER_SIZE;
		positionBuffer.clear();
	}

	ofxSJoint& operator=(const ofPoint& p);

	ofPoint * getBufferedPosition();

	static ofxFloatSlider errorTolerance;

protected:
	vector<ofPoint> positionBuffer;
	int idx;
	int N; //TODO should be changeable
	int pointsAdded;

	ofPoint bufferedPosition;
};


#endif /* OFXSJOINT_H_ */
