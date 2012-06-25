#ifndef OFXSKELETON_H_
#define OFXSKELETON_H_

#include "ofMain.h"
#include "ofxSLimb.h"
#include "ofxSJoint.h"
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
		return head.getBufferedPosition();
	}
	ofPoint * getNeck() {
		return neckToHead.getLimbStart(); //TODO not buffered
	}
	ofPoint * getLeftHand() {
		return leftHand.getBufferedPosition();
	}
	ofPoint * getRightHand() {
		return rightHand.getBufferedPosition();
	}
	ofPoint * getLeftElbow() {
		return elbow[0].getBufferedPosition();
	}
	ofPoint * getRightElbow() {
		return elbow[1].getBufferedPosition();
	}
	ofPoint * getLeftUpperTorso() {
		return leftUpperTorso.getBufferedPosition();
	}
	ofPoint * getRightUpperTorso() {
		return rightUpperTorso.getBufferedPosition();
	}

protected:
	int leftHandIdx;

	ofxSJoint head;
	ofxSJoint leftUpperTorso, rightUpperTorso;
	ofxSJoint elbow[2];
	ofxSJoint leftHand, rightHand;

	ofPoint upperTorsoFromBB[2];


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
