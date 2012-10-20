#include "ofxSkeleton.h"

void ofxSkeleton::resetFrame(){
	neckToHead.clear();
	arms[0].clear();
	arms[1].clear();
	legs[0].clear();
	legs[1].clear();
}

void ofxSkeleton::clear(){
	head.clear(6);
	neck.clear(6);
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
		headTop = *headTmp;
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
		float limbLength = arms[handIdx].calcLength();
		int idx = 0;
		ofPoint * p = arms[handIdx].getLinePoint(idx);
		float upperArmDistance = p->distance(upperTorsoFromPCA[index]);
		float armLength = limbLength + upperArmDistance;
		float v = upperArmDistance / armLength;
		ofPoint * last = &upperTorsoFromPCA[index];
		float lastV = 0;
		while(v < gui->lowerUpperArmRatio){
			last = p;
			lastV = v;
			++idx;
			p = arms[handIdx].getLinePoint(idx);
			upperArmDistance += p->distance(*last);
			v = upperArmDistance / armLength;
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
//		lowerTorso[0] = *p;
		lowerTorso[0] = gui->moveLowerTorso * (*p - lowerTorsoFromPCA[0]) + lowerTorsoFromPCA[0];
	}else{
		lowerTorso[0].bFound = false;
	}
}

void ofxSkeleton::locateRightLowerTorso(){
	ofPoint * p = legs[1-leftFootIdx].getLimbStart();
	if(p != NULL){
//		lowerTorso[1] = *p;
		lowerTorso[1] = gui->moveLowerTorso * (*p - lowerTorsoFromPCA[1]) + lowerTorsoFromPCA[1];
	}else{
		lowerTorso[1].bFound = false;
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

void ofxSkeleton::locateLeftKnee(){
	locateKnee(0);
}
void ofxSkeleton::locateRightKnee(){
	locateKnee(1);
}

void ofxSkeleton::locateKnee(int index){
	int footIdx = index;
	if(leftFootIdx == 1){
		footIdx = 1 - footIdx;
	}

	if(legs[footIdx].line != NULL){
		float limbLength = legs[footIdx].calcLength();
		int idx = 0;
		ofPoint * p = legs[footIdx].getLinePoint(idx);
		float upperDistance = p->distance(lowerTorso[index]);
		float armLength = limbLength + upperDistance;
		float v = upperDistance / armLength;
		ofPoint * last = &lowerTorso[index];
		float lastV = 0;
		while(v < gui->lowerUpperLegRatio){
			last = p;
			lastV = v;
			++idx;
			p = legs[footIdx].getLinePoint(idx);
			upperDistance += p->distance(*last);
			v = upperDistance / armLength;
		}

		knee[index] = ( (gui->lowerUpperLegRatio - lastV) / (v - lastV) ) * (*p-*last) + *last;

	}else{
		knee[index].bFound = false;
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
	locateLeftKnee();
	locateRightKnee();
}

void ofxSkeleton::draw(){
	ofSetColor(255,0,0,200);
	if( foundHead() ){
		ofEllipse(head,25,25);
//		ofEllipse(*getNeck(),15,15);
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
		ofLine(leftFoot,knee[0]);
		ofLine(lowerTorso[0],knee[0]);
		ofEllipse(leftFoot,25,25);
		ofEllipse(knee[0],20,20);
		ofEllipse(lowerTorso[0],15,15);
	}
	ofSetColor(150,220,0,200);
	if( true || legs[1].bFound ){
		ofLine(rightFoot,knee[1]);
		ofLine(lowerTorso[1],knee[1]);
		ofEllipse(rightFoot,25,25);
		ofEllipse(knee[1],20,20);
		ofEllipse(lowerTorso[1],15,15);
	}

	ofSetColor(0,255,255,200);
	ofLine(leftUpperTorso,lowerTorso[0]);
	ofLine(rightUpperTorso,lowerTorso[1]);
	ofLine(leftUpperTorso,rightUpperTorso);
	ofLine(lowerTorso[0],lowerTorso[1]);
}
