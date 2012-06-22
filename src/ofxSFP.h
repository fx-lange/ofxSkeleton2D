#ifndef OFXSFP_H_
#define OFXSFP_H_

#include "ofMain.h"

class ofxSFP : public ofPoint{
public:
	bool used;
	bool isTorsoPoint;

	float depth;

	ofxSFP(float x,float y,float z)
	:ofPoint(x,y,z)
	{
		used = false;
		isTorsoPoint = false;
		depth = 0;
	}
};

#endif
