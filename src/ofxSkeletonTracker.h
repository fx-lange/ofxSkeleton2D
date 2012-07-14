/*
 * ofxSkeleton2D.h
 *
 *  Created on: Jun 16, 2012
 *      Author: spta
 *
 *      //TODO locmax shader (and further cp sfp)
 */

#ifndef OFXSKELETONTRACKER_H_
#define OFXSKELETONTRACKER_H_

#include "ofMain.h"
#include "ofxVoronoiGL.h"
#include "ofxOpenCv.h"

#include "ofxSFPLine.h"
#include "ofxSkeleton.h"
#include "ofxSkeletonGui.h"

class ofxSkeletonTracker2D {
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

	ofxSkeleton & getSkeleton(){
		return skeleton;
	}

	void drawBinary(float x = 0, float y = 0);
	void drawDebugTorso(float x = 0, float y = 0);
	void drawDebugLines(float x = 0, float y = 0);
	void drawDebugLimbs(float x = 0, float y = 0);
	void drawDebugSkeleton(float x = 0, float y = 0);

	ofPolyline simpleContour;
protected:
	ofxSkeletonGui gui;
	float width, height;

	ofFbo fbo;

	ofxCvGrayscaleImage binary;
	ofxCvContourFinder contourFinder;

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
	ofVec2f sndTorsoDirection;
	ofPoint torsoHigh;
	ofPoint torsoLow;
	cv::Mat xss, yss; //TODO naming!!!

	//Lines
	vector<vector<ofxSFP> > linePixels;
	vector<ofxSFPLine> lines;

	//Limbs
	vector<ofxSLimb*> limbs; //TODO memory leak

	//SKELETON
	ofxSkeleton skeleton;

	// read pixels to PBO (asynchronous)
	GLubyte * readPixelsToPBO();
	// create fast grid like data structure to search for neighbors later
	void createSFPGrid(GLubyte * pboPtr);
	// pca on torso points
	void torsoPCA();

	void findLines();
	void mergeLines();

	void createLimbs();
	void locateLimbs();

	vector<ofxSFP*> findInit(ofxSFP * active, int manhattenRadius);
	ofxSFP * findNext(ofxSFP * active, int manhattenRadius);
	ofxSFP * findBest(ofxSFP * last, ofxSFP * active, int manhattenRadius);

};

#endif /* OFXSKELETONTRACKER_H_ */
