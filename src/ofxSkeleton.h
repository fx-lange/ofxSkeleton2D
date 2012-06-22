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

class ofxSkeleton {

public:
	ofxSLimb neckToHead,
		arms[2],
		legs[2];

	ofxSkeleton(){

		//limbs
//		neckToHead = NULL;
//		arms[0] = NULL;
//		arms[1] = NULL;
//		legs[0] = NULL;
//		legs[1] = NULL;

		//joints
	}

	void reset(){
		neckToHead.clear();
		arms[0].clear();
		arms[1].clear();
		legs[0].clear();
		legs[1].clear();
	}

	void draw(){
		ofSetColor(255,0,0,200);
		if(neckToHead.bFound ){
			ofEllipse(*getHead(),25,25);
			ofEllipse(*getNeck(),15,15);
		}
		ofSetColor(0,0,255,200);
		if( arms[0].bFound ){
			ofEllipse(arms[0].getLimbStart()->x,arms[0].getLimbStart()->y,15,15);
			ofEllipse(*getLeftHand(),25,25);
		}
		if( arms[1].bFound ){
			ofEllipse(arms[1].getLimbStart()->x,arms[1].getLimbStart()->y,15,15);
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


	ofPoint * getHead(){
		return neckToHead.getLimbEnd();
	}

	ofPoint * getNeck(){
		return neckToHead.getLimbStart();
	}

	ofPoint * getLeftHand(){
		return arms[0].getLimbEnd();
	}

	ofPoint * getRightHand(){
		return arms[1].getLimbEnd();
	}

};

#endif /* OFXSKELETON_H_ */
