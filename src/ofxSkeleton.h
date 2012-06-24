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
#include "ofxSkeletonGui.h"

class ofxSkeleton {

public:
	ofxSLimb neckToHead,
		arms[2],
		legs[2];

	ofRectangle torsoBB;

	int leftHandIdx;

	ofxSkeletonGui * gui;

	ofxSkeleton(){

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

	ofPoint * getLeftUpperTorso();
	ofPoint * getRightUpperTorso();
protected:
	ofPoint leftShoulder, rightShoulder;
	ofPoint elbow[2];
	ofPoint upperTorsoBB[2];

	ofPoint * getElbow(int idx);

};

#endif /* OFXSKELETON_H_ */
