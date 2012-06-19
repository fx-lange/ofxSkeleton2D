/*
 * ofxSkeleton2D.h
 *
 *  Created on: Jun 16, 2012
 *      Author: spta
 *
 *      //TODO locmax shader (and further cp sfp)
 */

#ifndef OFXSKELETON2D_H_
#define OFXSKELETON2D_H_

#include "ofMain.h"
#include "ofxVoronoiGL.h"
#include "ofxOpenCv.h"

#include "SFPLine.h"
#include "ofxSkeleton.h"
#include "ofxSkeletonGui.h"

class ofxSkeleton2D {
public:
	void setup(float width, float height);

	ofxPanel * getGui();
	ofxPanel * getImageprocessingPanel();
	ofxPanel * getShaderPanel();

	//generate skeleton feature points
	ofFbo * calcSfpAsFbo(ofxCvGrayscaleImage & grayInput, ofxCvGrayscaleImage & background);
	ofFbo * calcSfpAsFbo(ofxCvGrayscaleImage & binaryInput);
	ofFbo * calcSfpAsFbo(vector<ofPoint> & silhouette);

	void calcSkeleton();

	void drawVoronoi(float x = 0, float y = 0);
	ofxVoronoiGL & getVoronoi(){
		return voronoi;
	}

	void drawBinary(float x = 0, float y = 0);
	void drawDebugTorso(float x = 0, float y = 0);
	void drawDebugLines(float x = 0, float y = 0);
	void drawDebugLimbs(float x = 0, float y = 0);
	void drawDebugSkeleton(float x = 0, float y = 0);

protected:
	ofxSkeletonGui gui;
	float width, height;

	ofFbo fbo;

	ofxCvGrayscaleImage binary;
	ofxCvContourFinder contourFinder;
	ofPolyline simpleContour;

	ofxVoronoiGL voronoi;
	ofShader sfpShader;

	//pbo
	int activeIdx;
	GLuint pboIds[2];
	int nChannels;

	//vector of skeletonFeaturePoints
	list<ofPoint> sfpList; //TODO use class SFP

	//TORSO
	float maxDepth;
	ofShader findTorso;
	vector<ofPoint> torso;
	ofPoint center;
	ofVec2f primTorsoDirection;
	ofRectangle bbTorso;
	ofPoint torsoHigh;
	ofPoint torsoLow;
	cv::Mat xss, yss; //TODO naming!!!

	//Lines
	vector<vector<SFP> > linePixels;
	vector<SFPLine> lines;

	//Limbs
	vector<SLimb*> limbs; //TODO memory leak

	//SKELETON
	Skeleton skeleton;

	// read pixels to PBO (asynchronous)
	GLubyte * readPixelsToPBO();
	// create fast grid like data structure to search for neighbors later
	void createSFPGrid(GLubyte * pboPtr);
	// pca on torso points
	void torsoPCA();

	void findLines();
	void mergeLines();

	void createLimbs();
	void searchHeadAndArms();

	vector<SFP*> findInit(SFP * active, int manhattenRadius);
	SFP * findNext(SFP * active, int manhattenRadius);
	SFP * findBest(SFP * last, SFP * active, int manhattenRadius);

};

#endif /* OFXSKELETON2D_H_ */
