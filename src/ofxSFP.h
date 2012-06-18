#ifndef OFXSFP_H_
#define OFXSFP_H_

#include "ofMain.h"

class SFP : public ofPoint{
public:
	bool used;
	bool isTorsoPoint;
	SFP(float x,float y,float z)
	:ofPoint(x,y,z)
	{
		used = false;
		isTorsoPoint = false;
	}
};

#endif
