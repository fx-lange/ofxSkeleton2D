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
	ofPoint torsoCenter;

	ofxSkeletonGui * gui;

	ofxSkeleton() {
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

protected:
	int leftHandIdx, leftFootIdx;

	ofxSJoint head, neck;

	ofxSJoint leftUpperTorso, rightUpperTorso;
	ofxSJoint elbow[2];
	ofxSJoint leftHand, rightHand;

	ofxSJoint leftLowerTorso, rightLowerTorso;
	ofxSJoint leftFoot, rightFoot;


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

	void locateLeftLowerTorso();
	void locateRightLowerTorso();
};

#endif /* OFXSKELETON_H_ */
