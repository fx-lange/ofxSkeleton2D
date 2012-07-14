#ifndef OFXSJOINT_H_
#define OFXSJOINT_H_

#include "ofMain.h"

#define JOINT_BUFFER_SIZE 1

class ofxSJoint : public ofPoint{
public:
	bool bFound;
	//REVISIT each bFound = false should be tracked!

	ofxSJoint(){
		clear();
	}

	ofxSJoint(const ofPoint & p){
		x = p.x;
		y = p.y;
		z = p.z;

		positionBuffer.push_back(p);

		bFound = true;
	}

	void clear(){
		pointsAdded = idx = 0;
		bFound = false;
		N = JOINT_BUFFER_SIZE;
		positionBuffer.clear();
	}

	ofxSJoint& operator=(const ofPoint& p)
	{
	  if(pointsAdded > 30 && getBufferedPosition()->distance(p)>15000.f){ //TODO gui!
		  bFound = false;
		  return *this;
	  }
	  bFound = true;
	  x = p.x;
	  y = p.y;
	  z = p.z;

	  if(positionBuffer.size() < N){
		  positionBuffer.push_back(p);
	  }else{
		  positionBuffer[idx] = p;
	  }

	  idx = (idx + 1) % N;
	  ++pointsAdded;

	  return *this;
	}

	ofPoint * getBufferedPosition(){
		bufferedPosition.set(0,0);
		for(int i=0;i<positionBuffer.size();++i){
			bufferedPosition += positionBuffer[i];
		}
		bufferedPosition /= positionBuffer.size();
		return &bufferedPosition;
	}

protected:
	vector<ofPoint> positionBuffer;
	int idx;
	int N; //TODO should be changeable
	int pointsAdded;

	ofPoint bufferedPosition;
};


#endif /* OFXSJOINT_H_ */
