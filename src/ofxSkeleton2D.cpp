/*
 * ofxSkeleton2D.cpp
 *
 *  Created on: Jun 16, 2012
 *      Author: spta
 */

#include "ofxSkeleton2D.h"

void ofxSkeleton2D::setup(float _width, float _height){
	width = _width;
	height = _height;

	fbo.allocate(width,height,GL_RGBA);

	binary.allocate(width,height);

	voronoi.setup(width, height, 1.f);
	voronoi.setSkeletonMode(true);
	voronoi.setPerFeatureVD(true);

	sfpShader.load("shaders/discLocMax.vert", "shaders/discLocMax.frag");
}

ofFbo * ofxSkeleton2D::calcSfpAsFbo(ofxCvGrayscaleImage & grayInput,ofxCvGrayscaleImage & background, int threshold, float simplifyTolerance){
	binary.absDiff(background, grayInput);
	binary.threshold(threshold);
	return calcSfpAsFbo(binary, simplifyTolerance);
}

ofFbo * ofxSkeleton2D::calcSfpAsFbo(ofxCvGrayscaleImage & binaryInput, float simplifyTolerance){
	contourFinder.findContours(binaryInput, 20, (340 * height) / 3, 10, true); // find holes

	simpleContour.clear();
	if (!contourFinder.blobs.empty()) {
		ofxCvBlob & firstBlob = contourFinder.blobs[0];

		simpleContour.addVertexes(firstBlob.pts);
		simpleContour.simplify(simplifyTolerance);

		return calcSfpAsFbo(simpleContour.getVertices());
	}else{
		ofLog(OF_LOG_WARNING,"no silhouette found");
		return NULL;
	}
}

ofFbo * ofxSkeleton2D::calcSfpAsFbo(vector<ofPoint> & silhouette){
	voronoi.clear();
	voronoi.setPolygon(silhouette);
	//TODO contour points will be copied twice
	/* add method do work on them directly */

	voronoi.update();

	//VORONOI DISTANCE MAP -> SKELETON FEATURE POINTS
	//TODO Performance - momentan wird depthbuffer als texture übergeben - könnte als PBO auch schneller gehen

	fbo.begin();
	ofSetColor(0, 0, 0);
	ofFill();
	ofRect(0, 0, width, height); //TODO WORKARROUND - try with of0071
	ofSetColor(255, 255, 255);

	sfpShader.begin();

	sfpShader.setUniform1f("width", width);
	sfpShader.setUniform1f("height", height);
	sfpShader.setUniform1i("maxPlacement", 1); //TODO gui or change shader
	sfpShader.setUniformTexture("tex0", voronoi.getFbo().getDepthTexture(), voronoi.getFbo().getDepthTexture().getTextureData().textureID);
	sfpShader.setUniformTexture("colorTex", voronoi.getFbo().getTextureReference(), voronoi.getFbo().getTextureReference().getTextureData().textureID);
//	shader.setUniformTexture("tex0",voronoi.getFbo().getTextureReference(),voronoi.getFbo().getTextureReference().getTextureData().textureID);

	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);
	glVertex2d(0, 0);
	glTexCoord2f(width, 0);
	glVertex2d(width, 0);
	glTexCoord2f(width, height);
	glVertex2d(width, height);
	glTexCoord2f(0, height);
	glVertex2d(0, height);
	glEnd();

	sfpShader.end();
	fbo.end();

	return &fbo;
}


// ================================================================================
