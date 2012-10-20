/*
 * ofxPuppet.cpp
 *
 *  Created on: Jul 8, 2012
 *      Author: spta32
 */

#include "ofxPuppet.h"

ofxPuppet::ofxPuppet() {
	// TODO Auto-generated constructor stub

}

ofxPuppet::~ofxPuppet() {
	// TODO Auto-generated destructor stub
}

void ofxPuppet::drawLimb(ofPoint * lowerP, ofPoint * upperP, ofImage & image){
	if( lowerP != NULL && upperP != NULL){

		float length = lowerP->distance(*upperP);

		float scaleImage = length / (image.getHeight()-image.getWidth());

		ofVec3f vec = *lowerP - *upperP;
		float angle = vec.angle(ofVec3f(0,1));
		if(vec.angle(ofVec3f(1,0))<=90.f){
			angle *= -1;
		}
		vec /= 2.f;
		vec += *upperP;

		ofPushMatrix();
		ofTranslate(vec);
		ofRotate(angle);
		ofScale(scaleImage,scaleImage);
		image.draw(0,0);
		ofPopMatrix();
	}
}

void ofxPuppet::drawDummy(ofxSkeletonTracker2D * skeletonTracker){
		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofxSkeleton & skeleton = skeletonTracker->getSkeleton();

		//draw Mask
		//TODO rotation
//		ofPoint * headP = skeleton.getHeadTop();
//		ofPoint * neckP = skeleton.getNeck();
//		if(headP != NULL && neckP != NULL){
//			float headSize = headP->distance(*neckP);
//			float scaleHeadSize = headSize/headImg.getHeight() * scaleHead;
//			ofPoint headCenter = (*neckP - *headP) * 0.5f + *headP;
//			ofPushMatrix();
//			ofTranslate(headCenter);
//			ofScale(scaleHeadSize,scaleHeadSize);
//			headImg.draw(0,0);
//			ofPopMatrix();
//		}
			drawLimb(skeleton.getNeck(),skeleton.getHeadTop(),headImg);
			drawLimb(skeleton.getNeck(),skeleton.getHeadTop(),leftLowerArm);

		//left lower arm
			drawLimb(skeleton.getLeftHand(),skeleton.getLeftElbow(),leftLowerArm);

		//right lower arm
			drawLimb(skeleton.getRightHand(),skeleton.getRightElbow(),rightLowerArm);

		//left upperarm
			drawLimb(skeleton.getLeftElbow(),skeleton.getLeftUpperTorso(),leftUpperArm);

		//right upperarm
			drawLimb(skeleton.getRightElbow(),skeleton.getRightUpperTorso(),rightUpperArm);

		//left lower leg
			drawLimb(skeleton.getLeftFoot(),skeleton.getLeftKnee(),leftLowerLeg);

		//left upper leg
			drawLimb(skeleton.getLeftKnee(),skeleton.getLeftLowerTorso(),leftUpperLeg);

		//right lower leg
			drawLimb(skeleton.getRightFoot(),skeleton.getRightKnee(),rightLowerLeg);

		//right upper leg
			drawLimb(skeleton.getRightKnee(),skeleton.getRightLowerTorso(),rightUpperLeg);



		//breast
	//	ofRectangle & torso = skeleton.torsoBB;
	//	ofPushMatrix();
	//	float scaleTorsoSize = torso.height / torsoImg.getHeight() * scaleTorso;
	//	ofTranslate(skeleton.torsoCenter);
	//	ofScale(scaleTorsoSize,scaleTorsoSize);
	//	torsoImg.draw(0,0);
	//	ofPopMatrix();

		ofPoint * leftShoulderP = skeleton.getLeftUpperTorso();
		ofPoint * rightShoulderP = skeleton.getRightUpperTorso();
		if(leftShoulderP != NULL && rightShoulderP != NULL){
			ofPushMatrix();
			float torsoWidth = leftShoulderP->distance(*rightShoulderP);
			float scaleTorsoSize = torsoWidth / torsoImg.getWidth();// * scaleTorso;
			ofTranslate(leftShoulderP->x+torsoWidth/2.f,(leftShoulderP->y+rightShoulderP->y)*0.5f+scaleTorsoSize*torsoImg.getHeight()/2.f);
			scaleTorsoSize *= scaleTorso;
			ofScale(scaleTorsoSize,scaleTorsoSize);
			torsoImg.draw(0,0);
			ofPopStyle();
			ofPopMatrix();
		}
	}

void ofxPuppet::draw(ofxSkeletonTracker2D * skeletonTracker){
		ofPushStyle();
		ofSetRectMode(OF_RECTMODE_CENTER);
		ofxSkeleton & skeleton = skeletonTracker->getSkeleton();
		ofPoint * headP = skeleton.getHead();
		if(headP != NULL){
			float headSize = skeleton.getHeadSize();
			float scaleHeadSize = headSize/headImg.getHeight() * scaleHead;
			ofPushMatrix();
			ofTranslate(*headP);
			ofScale(scaleHeadSize,scaleHeadSize);
			headImg.draw(0,0);
			ofPopMatrix();
		}

		//left lower arm
			ofPoint * leftHandP = skeleton.getLeftHand();
			ofPoint * leftElbowP = skeleton.getLeftElbow();
			if( leftHandP != NULL && leftElbowP != NULL){
				float length = leftHandP->distance(*leftElbowP);
				float scaleLeftLowerArm = length / leftLowerArm.getHeight() * scaleLowerArm;

				ofVec3f vec = *leftHandP - *leftElbowP;
				float angle = vec.angle(ofVec3f(0,1));
				if(vec.angle(ofVec3f(1,0))<=90.f){
					angle *= -1;
				}
				vec *= 0.5f;
				vec += *leftElbowP;

				ofPushMatrix();
				ofTranslate(vec);
				ofRotate(angle);
				ofScale(scaleLeftLowerArm,scaleLeftLowerArm);
				leftLowerArm.draw(0,0);
				ofPopMatrix();
			}

			//right lower arm
			ofPoint * rightHandP = skeleton.getRightHand();
			ofPoint * rightElbowP = skeleton.getRightElbow();
			if( leftHandP != NULL && rightElbowP != NULL){
				float length = rightHandP->distance(*rightElbowP);
				float scaleRightLowerArm = length / rightLowerArm.getHeight() * scaleLowerArm;

				ofVec3f vec = *rightHandP - *rightElbowP;
				float angle = vec.angle(ofVec3f(0,1));
				if(vec.angle(ofVec3f(1,0))<=90.f){
					angle *= -1;
				}
				vec *= 0.5f;
				vec += *rightElbowP;

				ofPushMatrix();
				ofTranslate(vec);
				ofRotate(angle);
				ofScale(scaleRightLowerArm,scaleRightLowerArm);
				rightLowerArm.draw(0,0);
				ofPopMatrix();
			}

		//left upperarm
		ofPoint * leftShoulderP = skeleton.getLeftUpperTorso();
		if( leftShoulderP != NULL && leftElbowP != NULL){
			float length = leftShoulderP->distance(*leftElbowP);
			float scaleLeftArm = length / leftUpperArm.getHeight() * scaleUpperArm;

			ofVec3f vec = *leftElbowP - *leftShoulderP;
			float angle = vec.angle(ofVec3f(0,1));
			if(vec.angle(ofVec3f(1,0))<=90.f){
				angle *= -1;
			}
			vec *= 0.5f;
			vec += *leftShoulderP;

			ofPushMatrix();
			ofTranslate(vec);
			ofRotate(angle);
			ofScale(scaleLeftArm,scaleLeftArm);
			leftUpperArm.draw(0,0);
			ofPopMatrix();
		}

		//right upperarm
		ofPoint * rightShoulderP = skeleton.getRightUpperTorso();
		if( rightShoulderP != NULL && rightElbowP != NULL){
			float length = rightShoulderP->distance(*rightElbowP);
			float scaleRightArm = length / rightUpperArm.getHeight() * scaleUpperArm;

			ofVec3f vec = *rightElbowP - *rightShoulderP;
			float angle = vec.angle(ofVec3f(0,1));
			if(vec.angle(ofVec3f(1,0))<=90.f){
				angle *= -1;
			}
			vec *= 0.5f;
			vec += *rightShoulderP;

			ofPushMatrix();
			ofTranslate(vec);
			ofRotate(angle);
			ofScale(scaleRightArm,scaleRightArm);
			rightUpperArm.draw(0,0);
			ofPopMatrix();
		}

		//left lower leg
		ofPoint * p1 = skeleton.getLeftFoot();
		ofPoint * p2 = skeleton.getLeftKnee();
		if( p1 != NULL && p2 != NULL){
			float length = p1->distance(*p2);
			float scaleLimb = length / leftLowerLeg.getHeight() * scaleLowerLeg;

			ofVec3f vec = *p1 - *p2;
			float angle = vec.angle(ofVec3f(0,1));
			if(vec.angle(ofVec3f(1,0))<=90.f){
				angle *= -1;
			}
			vec *= 0.5f;
			vec += *p2;

			ofPushMatrix();
			ofTranslate(vec);
			ofRotate(angle);
			ofScale(scaleLimb,scaleLimb);
			leftLowerLeg.draw(0,0);
			ofPopMatrix();
		}

		//left upper leg
		p1 = skeleton.getLeftKnee();
		p2 = skeleton.getLeftLowerTorso();
		if( p1 != NULL && p2 != NULL){
			float length = p1->distance(*p2);
			float scaleLimb = length / leftUpperLeg.getHeight() * scaleUpperLeg;

			ofVec3f vec = *p1 - *p2;
			float angle = vec.angle(ofVec3f(0,1));
			if(vec.angle(ofVec3f(1,0))<=90.f){
				angle *= -1;
			}
			vec *= 0.5f;
			vec += *p2;

			ofPushMatrix();
			ofTranslate(vec);
			ofRotate(angle);
			ofScale(scaleLimb,scaleLimb);
			leftUpperLeg.draw(0,0);
			ofPopMatrix();
		}

		//right lower leg
		p1 = skeleton.getRightFoot();
		p2 = skeleton.getRightKnee();
		if( p1 != NULL && p2 != NULL){
			float length = p1->distance(*p2);
			float scaleLimb = length / rightLowerLeg.getHeight() * scaleLowerLeg;

			ofVec3f vec = *p1 - *p2;
			float angle = vec.angle(ofVec3f(0,1));
			if(vec.angle(ofVec3f(1,0))<=90.f){
				angle *= -1;
			}
			vec *= 0.5f;
			vec += *p2;

			ofPushMatrix();
			ofTranslate(vec);
			ofRotate(angle);
			ofScale(scaleLimb,scaleLimb);
			rightLowerLeg.draw(0,0);
			ofPopMatrix();
		}

		//right lower leg
		p1 = skeleton.getRightKnee();
		p2 = skeleton.getRightLowerTorso();
		if( p1 != NULL && p2 != NULL){
			float length = p1->distance(*p2);
			float scaleLimb = length / rightUpperLeg.getHeight() * scaleUpperLeg;

			ofVec3f vec = *p1 - *p2;
			float angle = vec.angle(ofVec3f(0,1));
			if(vec.angle(ofVec3f(1,0))<=90.f){
				angle *= -1;
			}
			vec *= 0.5f;
			vec += *p2;

			ofPushMatrix();
			ofTranslate(vec);
			ofRotate(angle);
			ofScale(scaleLimb,scaleLimb);
			rightUpperLeg.draw(0,0);
			ofPopMatrix();
		}



		//breast
	//	ofRectangle & torso = skeleton.torsoBB;
	//	ofPushMatrix();
	//	float scaleTorsoSize = torso.height / torsoImg.getHeight() * scaleTorso;
	//	ofTranslate(skeleton.torsoCenter);
	//	ofScale(scaleTorsoSize,scaleTorsoSize);
	//	torsoImg.draw(0,0);
	//	ofPopMatrix();

		ofPushMatrix();
		float torsoWidth = leftShoulderP->distance(*rightShoulderP);
		float scaleTorsoSize = torsoWidth / torsoImg.getWidth();// * scaleTorso;
		ofTranslate(leftShoulderP->x+torsoWidth/2.f,(leftShoulderP->y+rightShoulderP->y)*0.5f+scaleTorsoSize*torsoImg.getHeight()/2.f);
		scaleTorsoSize *= scaleTorso;
		ofScale(scaleTorsoSize,scaleTorsoSize);
		torsoImg.draw(0,0);
		ofPopStyle();
		ofPopMatrix();
	}

