#include "ofxSJoint.h"

ofxFloatSlider ofxSJoint::errorTolerance = ofxFloatSlider();

ofxSJoint& ofxSJoint::operator=(const ofPoint& p) {
	if (pointsAdded > 30 && getBufferedPosition()->distance(p) > errorTolerance) {
		bFound = false;
		return *this;
	}
	bFound = true;
	x = p.x;
	y = p.y;
	z = p.z;

	if ((int)positionBuffer.size() < N) {
		positionBuffer.push_back(p);
	} else {
		positionBuffer[idx] = p;
	}

	idx = (idx + 1) % N;
	++pointsAdded;

	return *this;
}

ofPoint * ofxSJoint::getBufferedPosition() {
	bufferedPosition.set(0, 0);
	for (int i = 0; i < (int)positionBuffer.size(); ++i) {
		bufferedPosition += positionBuffer[i];
	}
	bufferedPosition /= positionBuffer.size();
	return &bufferedPosition;
}
