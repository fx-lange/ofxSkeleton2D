/*
 * ofxPuppet.h
 *
 *  Created on: Jul 8, 2012
 *      Author: spta32
 */

#ifndef OFXPUPPET_H_
#define OFXPUPPET_H_

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSkeleton2D.h"

class ofxPuppet {
public:
	ofxFloatSlider scaleHead;
	ofxFloatSlider scaleTorso;
	ofxFloatSlider scaleLowerArm;
	ofxFloatSlider scaleUpperArm;
	ofxFloatSlider scaleLowerLeg;
	ofxFloatSlider scaleUpperLeg;

	ofImage headImg;
	ofImage torsoImg;

	ofImage leftLowerArm;
	ofImage rightLowerArm;
	ofImage leftUpperArm;
	ofImage rightUpperArm;

	ofImage leftLowerLeg;
	ofImage rightLowerLeg;
	ofImage leftUpperLeg;
	ofImage rightUpperLeg;

	ofxPuppet();
	virtual ~ofxPuppet();

	void addToGui(ofxPanel & gui){
		gui.add(scaleHead.setup("scale head",1,0,4));
		gui.add(scaleTorso.setup("scale torso",1,0,2));
		gui.add(scaleLowerArm.setup("scale lower arm",1,0,2));
		gui.add(scaleUpperArm.setup("scale upper arm",1,0,2));
		gui.add(scaleLowerLeg.setup("scale lower leg",1,0,2));
		gui.add(scaleUpperLeg.setup("scale upper leg",1,0,2));
	}

	void draw(ofxSkeletonTracker2D * skeletonTracker);
	void drawDummy(ofxSkeletonTracker2D * skeletonTracker);
protected:

	void drawLimb(ofPoint * lowerP, ofPoint * upperP, ofImage & image);
	void drawArms(ofxSkeleton & skeleton);
	void drawMask(ofxSkeleton & skeleton);

};

#endif /* OFXPUPPET_H_ */
