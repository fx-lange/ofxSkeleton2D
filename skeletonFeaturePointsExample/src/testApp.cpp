#include "testApp.h"

//--------------------------------------------------------------
void sfpExample::setup(){
	setupGui();

	vidPlayer.loadMovie("5Try.mov");
	vidPlayer.setLoopState(OF_LOOP_NORMAL);
	vidPlayer.play();

	width = vidPlayer.getWidth();
	height = vidPlayer.getHeight();

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

void sfpExample::setupGui(){
	gui.setup();
	gui.setSize(300,gui.getHeight());
	gui.add(threshold.setup("threshold",30,1,255));
	gui.add(tolerance.setup("simplify tolance",10,1,50));
}

//--------------------------------------------------------------
void sfpExample::update(){
	vidPlayer.idleMovie();

	if(vidPlayer.isFrameNew()){
		colorImg.setFromPixels(vidPlayer.getPixels(), width, height);
		grayImage = colorImg;

		fbo = skeletonTracker.calcSfpAsFbo(grayImage,grayBg,threshold,tolerance);
	}

	msg = "fps :" + ofToString(ofGetFrameRate());
}

//--------------------------------------------------------------
void sfpExample::draw(){
	if(fbo != NULL){
		fbo->draw(0,0);
	}

	ofDrawBitmapString(msg,width + 20,20);
}

//--------------------------------------------------------------
void sfpExample::keyPressed(int key){
}

//--------------------------------------------------------------
void sfpExample::keyReleased(int key){
}

//--------------------------------------------------------------
void sfpExample::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void sfpExample::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void sfpExample::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void sfpExample::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void sfpExample::windowResized(int w, int h){
}

