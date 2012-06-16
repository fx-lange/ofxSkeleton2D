/*
 * ofxSkeleton2D.h
 *
 *  Created on: Jun 16, 2012
 *      Author: spta
 */

#ifndef OFXSKELETON2D_H_
#define OFXSKELETON2D_H_

#include "ofMain.h"
#include "ofxVoronoiGL.h"
#include "ofxOpenCv.h"

class ofxSkeleton2D {
public:
	void setup(float width, float height);

	ofFbo * calcSfpAsFbo(ofxCvGrayscaleImage & grayInput, ofxCvGrayscaleImage & background, int threshold, float simplifyTolerance);
	ofFbo * calcSfpAsFbo(ofxCvGrayscaleImage & binaryInput, float simplifyTolerance);
	ofFbo * calcSfpAsFbo(vector<ofPoint> & silhouette);

protected:
	float width, height;

	ofFbo fbo;

	ofxCvGrayscaleImage binary;
	ofxCvContourFinder contourFinder;
	ofPolyline simpleContour;

	ofxVoronoiGL voronoi;
	ofShader sfpShader;
};

#endif /* OFXSKELETON2D_H_ */
