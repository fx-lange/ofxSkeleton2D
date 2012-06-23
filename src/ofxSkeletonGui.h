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
	ofxFloatSlider maxLimbPointDistance;

	//Torso
	ofxPanel torsoPanel;
	ofxToggle bExcludeTorso;
	ofxFloatSlider torsoThreshold;

	//Pose Estimation
	ofxPanel poseEstimationPanel;
	ofxFloatSlider maxAngleHead;

	ofxSkeletonGui(){
		panel.setup("ofxSkeleton2D","skeletonGui.xml");
//		panel.setSize(300,panel.getHeight());

		imagePanel.setup("image processing");
//		imagePanel.setSize(270,imagePanel.getHeight());
		imagePanel.add(threshold.setup("threshold",30,1,255));
		imagePanel.add(tolerance.setup("simplify tolance",10,1,50));
		panel.add(&imagePanel);

		shaderPanel.setup("shader");
		shaderPanel.add(lmpAccuracy.setup("loc max accuracy",0.0005,0.00001,0.01));
		panel.add(&shaderPanel);

		linePanel.setup("line construction");
//		linePanel.setSize(270,imagePanel.getHeight());
		linePanel.add(maxAngle.setup("max angle",15,0,90));
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
		mergePanel.add(maxMergeAngle.setup("max merge angle",10,1,50));
		mergePanel.add(maxLimbPointDistance.setup("max limb point dist",10,1,50));
		panel.add(&mergePanel);

		torsoPanel.setup("torso");
//		torsoPanel.setSize(270,imagePanel.getHeight());
		torsoPanel.add(bExcludeTorso.setup("exclude torso",true));
		torsoPanel.add(torsoThreshold.setup("torso threshold",0.7,0,1));
		panel.add(&torsoPanel);

		poseEstimationPanel.setup("pose estimation");
		poseEstimationPanel.add(maxAngleHead.setup("max angle head",30,1,90));
		panel.add(&poseEstimationPanel);
	}
};

#endif /* OFXSKELETONGUI_H_ */
