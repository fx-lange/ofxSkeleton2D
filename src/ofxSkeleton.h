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

class Skeleton {
public:
	Skeleton(){
		head = NULL;
		arms[0] = NULL;
		arms[1] = NULL;
		legs[0] = NULL;
		legs[1] = NULL;
	}
	SLimb * head,
		* arms[2],
		* legs[2];
	//POSES
	//		ofSetColor(255,0,0,200);
	//		ofEllipse(head.x,head.y,25,25);
	//		ofSetColor(0,0,255,200);
	//		ofEllipse(arms[0].x,arms[0].y,25,25);
	//		ofEllipse(arms[1].x,arms[1].y,25,25);
	void draw(){
		ofSetColor(255,0,0,200);
		if(head != NULL){
			ofEllipse(head->getLimbStart().x,head->getLimbStart().y,25,25);
		}
		ofSetColor(0,0,255,200);
		if( arms[0] != NULL){
			ofEllipse(arms[0]->getLimbStart().x,arms[0]->getLimbStart().y,25,25);
		}
		if( arms[1] != NULL){
			ofEllipse(arms[1]->getLimbStart().x,arms[1]->getLimbStart().y,25,25);
		}

		ofSetColor(0,255,0,200);
		if( legs[0] != NULL){
			ofEllipse(legs[0]->getLimbStart().x,legs[0]->getLimbStart().y,25,25);
		}
		if( legs[1] != NULL){
			ofEllipse(legs[1]->getLimbStart().x,legs[1]->getLimbStart().y,25,25);
		}
	}
};

#endif /* OFXSKELETON_H_ */
