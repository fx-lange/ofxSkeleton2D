/*
 * ofxSJoint.h
 *
 *  Created on: Jun 21, 2012
 *      Author: spta32
 */

#ifndef OFXSJOINT_H_
#define OFXSJOINT_H_

#include "ofMain.h"

class ofxSJoint : public ofPoint{
public:
	bool bFound;

	ofxSJoint(){
		bFound = false;
	}
};


#endif /* OFXSJOINT_H_ */
