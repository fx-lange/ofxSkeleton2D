#include "testApp.h"

//--------------------------------------------------------------
void skeleton2DExample::setup(){

	vidPlayer.loadMovie("5Try.mov");
	vidPlayer.setLoopState(OF_LOOP_NORMAL);
	vidPlayer.play();

	width = vidPlayer.getWidth();
	height = vidPlayer.getHeight();
	setupGui();

	skeletonTracker.setup(640,480);
	fbo = NULL;

	colorImg.allocate(width, height);
	grayImage.allocate(width, height);
	grayBg.allocate(width, height);

	//load background image
	background.loadImage("background.png");
	background.setImageType(OF_IMAGE_GRAYSCALE);
	bLearnBakground = false;
	grayBg.setFromPixels(background.getPixels(), width, height);
	//or use first frame as background
//	bLearnBakground = true;

	ofBackground(0);
	ofSetFrameRate(60);
}

void skeleton2DExample::setupGui(){
	gui.setup();
	gui.setPosition(width,height / 2 + 30);
	gui.setSize(300,gui.getHeight());
	gui.add(threshold.setup("threshold",30,1,255));
	gui.add(tolerance.setup("simplify tolance",10,1,50));
}

//--------------------------------------------------------------
void skeleton2DExample::update(){
	vidPlayer.idleMovie();

	if(vidPlayer.isFrameNew()){
		colorImg.setFromPixels(vidPlayer.getPixels(), width, height);
		grayImage = colorImg;

		fbo = skeletonTracker.calcSfpAsFbo(grayImage,grayBg,threshold,tolerance);
	}

	msg = "fps :" + ofToString(ofGetFrameRate());
	gui.draw();
}

//--------------------------------------------------------------
void skeleton2DExample::draw(){
	ofPushMatrix();
	ofScale(0.5,0.5);
	grayImage.draw(0,0);
	skeletonTracker.drawBinary(width,0);
	skeletonTracker.getVoronoi().getFbo().getDepthTexture().draw(width*2);
	ofPopMatrix();

	ofPushMatrix();
	ofTranslate(0,height/2);
	if(fbo != NULL){
		fbo->draw(0,0);
	}
	ofPopMatrix();

	gui.draw();
	ofDrawBitmapString(msg,width + 20,20);
}

//--------------------------------------------------------------
void skeleton2DExample::keyPressed(int key){
}

//--------------------------------------------------------------
void skeleton2DExample::keyReleased(int key){
}

//--------------------------------------------------------------
void skeleton2DExample::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void skeleton2DExample::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void skeleton2DExample::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void skeleton2DExample::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void skeleton2DExample::windowResized(int w, int h){
}

