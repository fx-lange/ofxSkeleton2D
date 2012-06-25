#include "ofxSkeleton.h"

void ofxSkeleton::reset(){
	neckToHead.clear();
	arms[0].clear();
	arms[1].clear();
	legs[0].clear();
	legs[1].clear();
}

void ofxSkeleton::calcHead(){
	ofPoint * headTmp = neckToHead.getLimbEnd();
	ofPoint * neckTmp = neckToHead.getLimbStart();
	if(headTmp != NULL && neckTmp != NULL){
		head = ((*neckTmp - *headTmp) * gui->headNeckRatio) + *headTmp;
	}else{
		head.bFound = false;
	}
}

void ofxSkeleton::locateLeftHand(){
	ofPoint * p = arms[leftHandIdx].getLimbEnd();
	if(p != NULL){
		leftHand = *p;
	}else{
		leftHand.bFound = false;
	}
}

void ofxSkeleton::locateRightHand(){
	ofPoint * p = arms[1-leftHandIdx].getLimbEnd();
	if(p != NULL){
		rightHand = *p;
	}else{
		rightHand.bFound = false;
	}
}

void ofxSkeleton::locateElbow(int index){
	int handIdx = index;
	if(leftHandIdx == 1){
		handIdx = 1 - handIdx;
	}

	int idx = -2;
	ofPoint * joint = arms[handIdx].getJoint(idx);
	if(joint != NULL){
		float lowerArmDistance = joint->distance(*arms[handIdx].getJoint(-1));
		float upperArmDistance = joint->distance(upperTorsoFromBB[index]);
		float v = upperArmDistance / (lowerArmDistance + upperArmDistance);
		float distance = lowerArmDistance;
		bool bEnd = false;
		ofPoint * last = arms[handIdx].getJoint(-1);
		float lastV = 1;
		while(v > gui->lowerUpperArmRatio){
			last = joint;
			lastV = v;
			--idx;
			joint = arms[handIdx].getJoint(idx);
			if(joint != NULL){
				distance = joint->distance(*last) + distance;
				upperArmDistance = joint->distance(upperTorsoFromBB[index]);
				v = upperArmDistance / (distance + upperArmDistance);
			}else{
				bEnd = true;
				break;
			}
		}

		if(bEnd){
			elbow[index] = (gui->lowerUpperArmRatio / lastV) * (*last-upperTorsoFromBB[index]) + upperTorsoFromBB[index];
		}else{
			elbow[index] = ( (gui->lowerUpperArmRatio - v) / (lastV - v) ) * (*last-*joint) + *joint;
		}

	}else{
		elbow[index].bFound = false;
	}
}

void ofxSkeleton::locateLeftElbow(){
	locateElbow(0);
}

void ofxSkeleton::locateRightElbow(){
	locateElbow(1);
}

void ofxSkeleton::locateLeftUpperTorso(){
	ofPoint leftUpperArm = elbow[0] - upperTorsoFromBB[0];
	leftUpperTorso = leftUpperArm * gui->torsoElbowRatio + upperTorsoFromBB[0];
	leftUpperTorso.bFound = elbow[0].bFound;
}

void ofxSkeleton::locateRightUpperTorso(){
	ofPoint rightUpperArm = elbow[1] - upperTorsoFromBB[1];
	rightUpperTorso = rightUpperArm * gui->torsoElbowRatio + upperTorsoFromBB[1];
	rightUpperTorso.bFound = elbow[1].bFound;
}

void ofxSkeleton::update(){

	upperTorsoFromBB[0].set(torsoBB.x,torsoBB.y);
	upperTorsoFromBB[1].set(torsoBB.x+torsoBB.width,torsoBB.y);

	leftHandIdx = 0;
	if(arms[0].getLimbEnd() != NULL && arms[1].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > arms[1].getLimbEnd()->x){
			leftHandIdx = 1;
		}
	}else if(arms[0].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > upperTorsoFromBB[1].x){
			leftHandIdx = 1;
		}
	}else if(arms[1].getLimbEnd() != NULL){
		if(arms[1].getLimbEnd()->x < upperTorsoFromBB[0].x ){
			leftHandIdx = 1;
		}
	}

	calcHead();
	locateLeftHand();
	locateRightHand();
	locateLeftElbow();
	locateRightElbow();
	locateLeftUpperTorso();
	locateRightUpperTorso();
}

void ofxSkeleton::draw(){
	ofSetColor(255,0,0,200);
	if( foundHead() ){
		ofEllipse(head,25,25);
		ofEllipse(*getNeck(),15,15);
	}
	ofSetColor(0,0,255,200);
	if( foundLeftArm() ){
		ofEllipse(leftHand,25,25);
		ofEllipse(elbow[0],20,20);
		ofEllipse(leftUpperTorso,15,15);
	}
	if( foundRightArm() ){
		ofEllipse(rightHand,25,25);
		ofEllipse(elbow[1],20,20);
		ofEllipse(rightUpperTorso,15,15);
	}

	ofSetColor(0,255,0,200);
	if( legs[0].bFound ){
		ofEllipse(legs[0].getLimbStart()->x,legs[0].getLimbStart()->y,15,15);
	}
	if( legs[1].bFound ){
		ofEllipse(legs[1].getLimbStart()->x,legs[1].getLimbStart()->y,15,15);
	}
}
