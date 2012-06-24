#include "ofxSkeleton.h"

void ofxSkeleton::reset(){
	neckToHead.clear();
	arms[0].clear();
	arms[1].clear();
	legs[0].clear();
	legs[1].clear();
}

ofPoint * ofxSkeleton::getLeftHand(){
	return arms[leftHandIdx].getLimbEnd();
}

ofPoint * ofxSkeleton::getRightHand(){
	return arms[1-leftHandIdx].getLimbEnd();
}

ofPoint * ofxSkeleton::getElbow(int index){
	int handIdx = index;
	if(leftHandIdx == 1){
		handIdx = 1 - handIdx;
	}

	int idx = -2;
	ofPoint * joint = arms[handIdx].getJoint(idx);
	if(joint != NULL){
		//TODO if no elbow joint found - just hand and "shoulder"

		float lowerArmDistance = joint->distance(*arms[handIdx].getJoint(-1));
		float upperArmDistance = joint->distance(upperTorsoBB[index]);
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
				upperArmDistance = joint->distance(upperTorsoBB[index]);
				v = upperArmDistance / (distance + upperArmDistance);
			}else{
				bEnd = true;
				break;
			}
		}

		if(bEnd){
			elbow[index] = (gui->lowerUpperArmRatio / lastV) * (*last-upperTorsoBB[index]) + upperTorsoBB[index];
		}else{
			elbow[index] = ( (gui->lowerUpperArmRatio - v) / (lastV - v) ) * (*last-*joint) + *joint;
		}

		return &elbow[index];
	}
	return NULL;
}

ofPoint * ofxSkeleton::getLeftElbow(){
	return getElbow(0);
}

ofPoint * ofxSkeleton::getRightElbow(){
	return getElbow(1);
}

ofPoint * ofxSkeleton::getLeftUpperTorso(){
	ofPoint * elbow = getLeftElbow();
	if(elbow != NULL){
		ofPoint leftUpperArm = *elbow - upperTorsoBB[0];
		leftShoulder = leftUpperArm * gui->torsoElbowRatio + upperTorsoBB[0];
		return &leftShoulder;
	}
	return NULL;
}

ofPoint * ofxSkeleton::getRightUpperTorso(){
	ofPoint * elbow = getRightElbow();
	if(elbow != NULL){
		ofPoint rightUpperArm = *elbow - upperTorsoBB[1];
		rightShoulder = rightUpperArm * gui->torsoElbowRatio + upperTorsoBB[1];
		return &rightShoulder;
	}
	return NULL;
}

void ofxSkeleton::update(){

	upperTorsoBB[0].set(torsoBB.x,torsoBB.y);
	upperTorsoBB[1].set(torsoBB.x+torsoBB.width,torsoBB.y);

	leftHandIdx = 0;
	if(arms[0].getLimbEnd() != NULL && arms[1].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > arms[1].getLimbEnd()->x){
			leftHandIdx = 1;
		}
	}else if(arms[0].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > upperTorsoBB[1].x){
			leftHandIdx = 1;
		}
	}else if(arms[1].getLimbEnd() != NULL){
		if(arms[1].getLimbEnd()->x < upperTorsoBB[0].x ){
			leftHandIdx = 1;
		}
	}
}

void ofxSkeleton::draw(){
	ofSetColor(255,0,0,200);
	if(neckToHead.bFound ){
		ofEllipse(*getHead(),25,25);
		ofEllipse(*getNeck(),15,15);
	}
	ofSetColor(0,0,255,200);
	if( arms[leftHandIdx].bFound ){
//		ofEllipse(arms[0].getLimbStart()->x,arms[0].getLimbStart()->y,15,15);
		ofEllipse(*getLeftHand(),25,25);
		ofLine(getLeftElbow()->x,getLeftElbow()->y,torsoBB.x,torsoBB.y);
		ofPoint * shoulder = getLeftUpperTorso();
		if(shoulder != NULL){
			ofEllipse(*shoulder,15,15);
		}
	}
	if( arms[1-leftHandIdx].bFound ){
//		ofEllipse(arms[1].getLimbStart()->x,arms[1].getLimbStart()->y,15,15);
		ofEllipse(*getRightHand(),25,25);
		ofLine(getRightElbow()->x,getRightElbow()->y,torsoBB.x+torsoBB.width,torsoBB.y);
		ofPoint * shoulder = getRightUpperTorso();
		if(shoulder != NULL){
			ofEllipse(*shoulder,15,15);
		}
	}

	ofSetColor(0,255,0,200);
	if( legs[0].bFound ){
		ofEllipse(legs[0].getLimbStart()->x,legs[0].getLimbStart()->y,15,15);
	}
	if( legs[1].bFound ){
		ofEllipse(legs[1].getLimbStart()->x,legs[1].getLimbStart()->y,15,15);
	}
}
