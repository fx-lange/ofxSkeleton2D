#ifndef OFXSKELETON_H_
#define OFXSKELETON_H_

#include "ofMain.h"
#include "ofxSLimb.h"
#include "ofxSJoint.h"
#include "ofxSkeletonGui.h"

class ofxSkeleton {
	//TODO ignore errors ?!
	//TODO use average head size to improve pose estimation
	//TODO head position is not completely buffered (cause of neck) - and his size too
	//REVISIT improve inner joings by using original SFPLine Points instead of joints?!

public:
	//TODO should be protected - possible misuse
	ofxSLimb neckToHead, arms[2], legs[2];

	ofPoint upperTorsoFromPCA[2];
	ofPoint lowerTorsoFromPCA[2];
	ofPoint torsoCenter;

	ofxSkeletonGui * gui;

	ofxSkeleton(){
		head.N = 12;
		neck.N = 12;
	}

	void resetFrame();
	void clear();

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

	ofPoint * getHeadTop(){
		return headTop.getBufferedPosition();
	}

	float getHeadSize(){
		return head.getBufferedPosition()->distance(*neck.getBufferedPosition()) * 2.f;
	}
	ofPoint * getNeck() {
		return neck.getBufferedPosition();
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

	ofPoint * getLeftFoot() {
		return leftFoot.getBufferedPosition();
	}
	ofPoint * getRightFoot() {
		return rightFoot.getBufferedPosition();
	}
	ofPoint * getLeftKnee() {
		return knee[0].getBufferedPosition();
	}
	ofPoint * getRightKnee() {
		return knee[1].getBufferedPosition();
	}
	ofPoint * getLeftLowerTorso() {
		return lowerTorso[0].getBufferedPosition();
	}
	ofPoint * getRightLowerTorso() {
		return lowerTorso[1].getBufferedPosition();
	}

protected:
	int leftHandIdx, leftFootIdx;

	ofxSJoint head, neck, headTop;

	ofxSJoint leftUpperTorso, rightUpperTorso;
	ofxSJoint elbow[2];
	ofxSJoint leftHand, rightHand;

	ofxSJoint lowerTorso[2];
	ofxSJoint leftFoot, rightFoot;
	ofxSJoint knee[2];


	void locateHead();

	void locateLeftHand();
	void locateRightHand();

	void locateElbow(int idx);
	void locateLeftElbow();
	void locateRightElbow();

	void locateLeftUpperTorso();
	void locateRightUpperTorso();

	void locateLeftFoot();
	void locateRightFoot();

	void locateKnee(int idx);
	void locateLeftKnee();
	void locateRightKnee();

	void locateLeftLowerTorso();
	void locateRightLowerTorso();
};

#endif /* OFXSKELETON_H_ */
