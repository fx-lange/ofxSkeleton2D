#ifndef OFXSKELETON_H_
#define OFXSKELETON_H_

#include "ofMain.h"
#include "ofxSLimb.h"
#include "ofxSkeletonGui.h"

class ofxSkeleton {
	//TODO calc average head size
	//TODO use average head size to improve pose estimation
	//TODO use buffer for positions
	//REVISIT improve inner joings by using original SFPLine Points instead of joints?!

public:
	//TODO should be protected - possible misuse
	ofxSLimb neckToHead, arms[2], legs[2];

	ofRectangle torsoBB;
	ofPoint torsoCenter;

	ofxSkeletonGui * gui;

	ofxSkeleton() {
	}

	void reset();
	void update();
	void draw();

	bool foundHead() {
		return neckToHead.bFound;
	}

	bool foundLeftArm() {
		return arms[leftHandIdx].bFound;
	}

	bool foundRightArm() {
		return arms[1 - leftHandIdx].bFound;
	}

	ofPoint * getHead() {
		return &head;
	}
	ofPoint * getNeck() {
		return neckToHead.getLimbStart();
	}
	ofPoint * getLeftHand() {
		return &leftHand;
	}
	ofPoint * getRightHand() {
		return &rightHand;
	}
	ofPoint * getLeftElbow() {
		return &elbow[0];
	}
	ofPoint * getRightElbow() {
		return &elbow[1];
	}
	ofPoint * getLeftUpperTorso() {
		return &leftUpperTorso;
	}
	ofPoint * getRightUpperTorso() {
		return &rightUpperTorso;
	}

protected:
	int leftHandIdx;

	ofPoint head;
	ofPoint leftUpperTorso, rightUpperTorso;
	ofPoint elbow[2];
	ofPoint upperTorsoFromBB[2];
	ofPoint leftHand, rightHand;


	void calcHead();

	void locateLeftHand();
	void locateRightHand();

	void locateElbow(int idx);
	void locateLeftElbow();
	void locateRightElbow();

	void locateLeftUpperTorso();
	void locateRightUpperTorso();
};

#endif /* OFXSKELETON_H_ */
