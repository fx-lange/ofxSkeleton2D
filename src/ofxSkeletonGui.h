/*
 * ofxSkeletonGui.h
 *
 *  Created on: Jun 18, 2012
 *      Author: spta
 *
 *      TODO differnt detailed guis for different application areas
 */

#ifndef OFXSKELETONGUI_H_
#define OFXSKELETONGUI_H_

#include "ofMain.h"
#include "ofxGui.h"

class ofxSkeletonGui {
public:

	ofxPanel panel;

	ofxPanel imagePanel;
	ofxFloatSlider threshold;
	ofxFloatSlider tolerance;
	ofxFloatSlider camFarClipFactor;

	ofxPanel shaderPanel;
	ofxFloatSlider lmpAccuracy;

	//Find Line Segments
	ofxPanel linePanel;
	ofxFloatSlider maxAngle;
	ofxIntSlider minCountPixelsPerLine;
	ofxIntSlider manhattenRadius;
	ofxToggle bEasyStartAngle;
	ofxIntSlider easyStartFaktor;
	ofxToggle bFindBest; //or what?
	ofxToggle bUseFindInit;
	ofxToggle bSecondChance;
	ofxIntSlider maxChances;

	//Merge lines
	ofxPanel mergePanel;
	ofxToggle bMergeLines;
	ofxFloatSlider maxMergeDistance;
	ofxFloatSlider maxMergeAngle;

	//Torso
	ofxPanel torsoPanel;
	ofxToggle bExcludeTorso;
	ofxFloatSlider torsoThreshold;
	ofxFloatSlider scaleLambda1;
	ofxFloatSlider scaleLambda1Low;
	ofxFloatSlider scaleLambda2;

	//Pose Estimation
	ofxPanel poseEstimationPanel;
	ofxFloatSlider maxAngleHead;
	ofxFloatSlider maxLimbPointDistance;
	ofxFloatSlider minlineLength;
	ofxFloatSlider minLimbLengthSum;
	ofxFloatSlider torsoElbowRatio;
	ofxFloatSlider moveLowerTorso;
	ofxFloatSlider lowerUpperArmRatio;
	ofxFloatSlider lowerUpperLegRatio;
	ofxFloatSlider headNeckRatio;
	ofxToggle betweenShoulders;

	//PostProcessing

	ofxSkeletonGui(){
		panel.setup("ofxSkeleton2D","skeletonGui.xml");
//		panel.setSize(300,panel.getHeight());

		imagePanel.setup("image processing");
//		imagePanel.setSize(270,imagePanel.getHeight());
		imagePanel.add(threshold.setup("threshold",30,1,255));
		imagePanel.add(tolerance.setup("simplify tolance",10,1,50));
		imagePanel.add(camFarClipFactor.setup("far clip factor",0.5,.1,1.0));
		panel.add(&imagePanel);

		shaderPanel.setup("shader");
		shaderPanel.add(lmpAccuracy.setup("loc max accuracy",0.0005,0.00001,0.01));
		panel.add(&shaderPanel);

		linePanel.setup("line construction");
//		linePanel.setSize(270,imagePanel.getHeight());
		linePanel.add(maxAngle.setup("max angle",15,0,180));
		linePanel.add(minCountPixelsPerLine.setup("min pixels per line",5,1,30));
		linePanel.add(manhattenRadius.setup("max line point dist",5,1,30));
		linePanel.add(bEasyStartAngle.setup("easy start",false));
		linePanel.add(easyStartFaktor.setup("max angle multiplier",1,1,10));
		linePanel.add(bSecondChance.setup("second chance",false));
		linePanel.add(maxChances.setup("max chances",1,1,10));
		linePanel.add(bFindBest.setup("use findBest",true));
		linePanel.add(bUseFindInit.setup("use find init",true));
		panel.add(&linePanel);

		mergePanel.setup("merge lines");
//		mergePanel.setSize(270,imagePanel.getHeight());
		mergePanel.add(bMergeLines.setup("merge them",false));
		mergePanel.add(maxMergeDistance.setup("max merge distance",10,1,50));
		mergePanel.add(maxMergeAngle.setup("max merge angle",10,1,180));
		panel.add(&mergePanel);

		torsoPanel.setup("torso");
//		torsoPanel.setSize(270,imagePanel.getHeight());
		torsoPanel.add(bExcludeTorso.setup("exclude torso",true));
		torsoPanel.add(torsoThreshold.setup("torso threshold",0.7,0,1));
		torsoPanel.add(scaleLambda1.setup("scale lambda1",1,1,3));
		torsoPanel.add(scaleLambda1Low.setup("scale lambda1 low",1,1,3));
		torsoPanel.add(scaleLambda2.setup("scale lambda2",1,1,3));
		panel.add(&torsoPanel);

		poseEstimationPanel.setup("pose estimation");
		poseEstimationPanel.add(maxLimbPointDistance.setup("max limb point dist",10,1,50));
		poseEstimationPanel.add(minlineLength.setup("min line length",20,1,50));
		poseEstimationPanel.add(minLimbLengthSum.setup("min limb length",40,1,100));
		poseEstimationPanel.add(maxAngleHead.setup("max angle head",30,1,90));
		poseEstimationPanel.add(headNeckRatio.setup("head neck ratio",0.2,0,1));
		poseEstimationPanel.add(torsoElbowRatio.setup("torso elbow ratio",0.2,0,1));
		poseEstimationPanel.add(moveLowerTorso.setup("move lower torso",0.2,0,1));
		poseEstimationPanel.add(lowerUpperArmRatio.setup("low upper ratio",0.5,0,1));
		poseEstimationPanel.add(lowerUpperLegRatio.setup("low upper leg ratio",0.5,0,1));
		poseEstimationPanel.add(betweenShoulders.setup("between shoulders",true));
		poseEstimationPanel.add(ofxSJoint::errorTolerance.setup("joint error tolerance",150,50,1500));
		panel.add(&poseEstimationPanel);
	}
};

#endif /* OFXSKELETONGUI_H_ */
