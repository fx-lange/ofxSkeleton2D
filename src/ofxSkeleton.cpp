#include "ofxSkeleton.h"

void ofxSkeleton::resetFrame(){
	neckToHead.clear();
	arms[0].clear();
	arms[1].clear();
	legs[0].clear();
	legs[1].clear();
}

void ofxSkeleton::clear(){
	head.clear();
	neck.clear();
	leftUpperTorso.clear();
	rightUpperTorso.clear();
	elbow[0].clear();
	elbow[1].clear();
	leftHand.clear();
	rightHand.clear();
}

void ofxSkeleton::locateHead(){
	ofPoint * headTmp = neckToHead.getLimbEnd();
	ofPoint * neckTmp = neckToHead.getLimbStart();
	if(headTmp != NULL && neckTmp != NULL){
		head = ((*neckTmp - *headTmp) * gui->headNeckRatio) + *headTmp;
		neck = *neckTmp;
	}else{
		head.bFound = false;
		neck.bFound = false;
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

	if(arms[handIdx].line != NULL){
		int idx = 0;
		ofPoint * p = arms[handIdx].getLinePoint(idx);
		ofPoint * end = arms[handIdx].getLinePoint(-1);
		float lowerArmDistance = p->distance(*end);
		float upperArmDistance = p->distance(upperTorsoFromPCA[index]);
		float v = upperArmDistance / (lowerArmDistance + upperArmDistance);
		ofPoint * last = &upperTorsoFromPCA[index];
		float lastV = 0;
		while(v < gui->lowerUpperArmRatio){
			last = p;
			lastV = v;
			++idx;
			p = arms[handIdx].getLinePoint(idx);
			upperArmDistance = p->distance(upperTorsoFromPCA[index]);
			lowerArmDistance = p->distance(*end);
			v = upperArmDistance / (lowerArmDistance + upperArmDistance);
		}

		elbow[index] = ( (gui->lowerUpperArmRatio - lastV) / (v - lastV) ) * (*p-*last) + *last;

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
	ofPoint leftUpperArm = elbow[0] - upperTorsoFromPCA[0];
	leftUpperTorso = leftUpperArm * gui->torsoElbowRatio + upperTorsoFromPCA[0];
	leftUpperTorso.bFound = elbow[0].bFound;
}

void ofxSkeleton::locateRightUpperTorso(){
	ofPoint rightUpperArm = elbow[1] - upperTorsoFromPCA[1];
	rightUpperTorso = rightUpperArm * gui->torsoElbowRatio + upperTorsoFromPCA[1];
	rightUpperTorso.bFound = elbow[1].bFound;
}

void ofxSkeleton::locateLeftLowerTorso(){
	ofPoint * p = legs[leftFootIdx].getLimbStart();
	if(p != NULL){
		leftLowerTorso = *p;
	}else{
		leftLowerTorso.bFound = false;
	}
}

void ofxSkeleton::locateRightLowerTorso(){
	ofPoint * p = legs[1-leftFootIdx].getLimbStart();
	if(p != NULL){
		rightLowerTorso = *p;
	}else{
		rightLowerTorso.bFound = false;
	}
}

void ofxSkeleton::locateLeftFoot(){
	ofPoint * p = legs[leftFootIdx].getLimbEnd();
	if(p != NULL){
		leftFoot = *p;
	}else{
		leftFoot.bFound = false;
	}
}

void ofxSkeleton::locateRightFoot(){
	ofPoint * p = legs[1-leftFootIdx].getLimbEnd();
	if(p != NULL){
		rightFoot = *p;
	}else{
		rightFoot.bFound = false;
	}
}


void ofxSkeleton::update(){

	leftHandIdx = 0;
	if(arms[0].getLimbEnd() != NULL && arms[1].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > arms[1].getLimbEnd()->x){
			leftHandIdx = 1;
		}
	}else if(arms[0].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > upperTorsoFromPCA[1].x){
			leftHandIdx = 1;
		}
	}else if(arms[1].getLimbEnd() != NULL){
		if(arms[1].getLimbEnd()->x < upperTorsoFromPCA[0].x ){
			leftHandIdx = 1;
		}
	}

	leftFootIdx = 0;
	if(legs[0].getLimbEnd() != NULL && legs[1].getLimbEnd() != NULL){
		if(legs[0].getLimbEnd()->x > legs[1].getLimbEnd()->x){
			leftFootIdx = 1;
		}
	}else if(legs[0].getLimbEnd() != NULL){
		if(legs[0].getLimbEnd()->x > upperTorsoFromPCA[1].x){
			leftFootIdx = 1;
		}
	}else if(legs[1].getLimbEnd() != NULL){
		if(legs[1].getLimbEnd()->x < upperTorsoFromPCA[0].x ){
			leftFootIdx = 1;
		}
	}

	locateHead();

	locateLeftHand();
	locateRightHand();
	locateLeftElbow();
	locateRightElbow();
	locateLeftUpperTorso();
	locateRightUpperTorso();

	locateLeftFoot();
	locateRightFoot();
	locateLeftLowerTorso();
	locateRightLowerTorso();
}

void ofxSkeleton::draw(){
	ofSetColor(255,0,0,200);
	if( foundHead() ){
		ofEllipse(head,25,25);
		ofEllipse(*getNeck(),15,15);
	}
	ofSetColor(0,0,255,200);
	if( true || foundLeftArm() ){
		ofLine(leftHand,elbow[0]);
		ofLine(leftUpperTorso,elbow[0]);
		ofEllipse(leftHand,25,25);
		ofEllipse(elbow[0],20,20);
		ofEllipse(leftUpperTorso,15,15);
	}
	ofSetColor(100,0,255,200);
	if( true || foundRightArm() ){
		ofLine(rightHand,elbow[1]);
		ofLine(rightUpperTorso,elbow[1]);
		ofEllipse(rightHand,25,25);
		ofEllipse(elbow[1],20,20);
		ofEllipse(rightUpperTorso,15,15);
	}

	ofSetColor(0,200,0,200);
	if( true || legs[0].bFound ){
		ofEllipse(leftFoot,25,25);
		ofEllipse(leftLowerTorso,15,15);
	}
	ofSetColor(100,200,0,200);
	if( true || legs[1].bFound ){
		ofEllipse(rightFoot,25,25);
		ofEllipse(rightLowerTorso,15,15);
	}

	ofSetColor(0,255,255,200);
	ofLine(leftUpperTorso,leftLowerTorso);
	ofLine(rightUpperTorso,rightLowerTorso);
	ofLine(leftUpperTorso,rightUpperTorso);
	ofLine(leftLowerTorso,rightLowerTorso);
}
