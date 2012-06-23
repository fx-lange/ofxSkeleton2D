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

ofPoint * ofxSkeleton::getLeftElbow(){
	//TODO if tracker finds a wrist joint :|
	ofPoint * joint = arms[leftHandIdx].getJoint(-2);
	if(joint != NULL){
		//TODO if no elbow joint found - just hand and "shoulder"
	}
	return arms[leftHandIdx].getJoint(-2);
}

ofPoint * ofxSkeleton::getRightElbow(){
	//TODO if tracker finds a wrist joint :|
	ofPoint * joint = arms[1-leftHandIdx].getJoint(-2);
	return arms[1-leftHandIdx].getJoint(-2);
}

void ofxSkeleton::update(){
	leftHandIdx = 0;
	if(arms[0].getLimbEnd() != NULL && arms[1].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > arms[1].getLimbEnd()->x){
			leftHandIdx = 1;
		}
	}else if(arms[0].getLimbEnd() != NULL){
		if(arms[0].getLimbEnd()->x > torsoBB.x + torsoBB.width){
			leftHandIdx = 1;
		}
	}else if(arms[1].getLimbEnd() != NULL){
		if(arms[1].getLimbEnd()->x < torsoBB.x ){
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
	}
	if( arms[1-leftHandIdx].bFound ){
//		ofEllipse(arms[1].getLimbStart()->x,arms[1].getLimbStart()->y,15,15);
		ofEllipse(*getRightHand(),25,25);
	}

	ofSetColor(0,255,0,200);
	if( legs[0].bFound ){
		ofEllipse(legs[0].getLimbStart()->x,legs[0].getLimbStart()->y,15,15);
	}
	if( legs[1].bFound ){
		ofEllipse(legs[1].getLimbStart()->x,legs[1].getLimbStart()->y,15,15);
	}
}
