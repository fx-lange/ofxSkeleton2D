#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxSkeleton2D.h"

class skeleton2DExample: public ofBaseApp {

public:
	void setup();
	void setupGui();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);

	float width, height;

	bool bLearnBakground;

	ofxCvColorImage colorImg;
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofImage background;

	ofVideoPlayer vidPlayer;

	ofxSkeleton2D skeletonTracker;
	ofFbo * fbo;

	//GUI
	ofxPanel gui;
	ofxFloatSlider threshold;
	ofxFloatSlider tolerance;

	string msg;
};
