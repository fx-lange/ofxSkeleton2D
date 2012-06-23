/*
 * ofxSkeleton.h
 *
 *  Created on: Jun 18, 2012
 *      Author: spta
 */

#ifndef OFXSKELETON_H_
#define OFXSKELETON_H_

#include "ofMain.h"
#include "ofxSLimb.h"

class ofxSkeleton {

public:
	ofxSLimb neckToHead,
		arms[2],
		legs[2];

	ofRectangle torsoBB;

	int leftHandIdx;

	ofxSkeleton(){

		//limbs
//		neckToHead = NULL;
//		arms[0] = NULL;
//		arms[1] = NULL;
//		legs[0] = NULL;
//		legs[1] = NULL;

		//joints
	}

	void reset();

	void update();
	void draw();


	ofPoint * getHead(){
		return neckToHead.getLimbEnd();
	}

	ofPoint * getNeck(){
		return neckToHead.getLimbStart();
	}

	ofPoint * getLeftHand();
	ofPoint * getRightHand();

	ofPoint * getLeftElbow();
	ofPoint * getRightElbow();

};

#endif /* OFXSKELETON_H_ */
