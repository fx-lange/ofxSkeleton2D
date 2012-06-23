#ifndef OFXSFP_H_
#define OFXSFP_H_

#include "ofMain.h"

class ofxSFP : public ofPoint{
public:
	bool used;
	bool isTorsoPoint;

	float depth;

	vector<ofxSFP *> beatenNeighbors;

	ofxSFP(float x,float y,float z)
	:ofPoint(x,y,z)
	{
		used = false;
		isTorsoPoint = false;
		depth = 0;
	}

	void freeBeatenNeighbors(){
		for(int i=0;i<beatenNeighbors.size();++i){
			beatenNeighbors[i]->used = false;
		}
	}
};

#endif
