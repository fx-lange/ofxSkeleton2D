#include "ofxSLimb.h"


ofxSLimb::ofxSLimb(ofPoint & p1, ofPoint & p2){
	joints.clear();
	joints.push_back(p1);
	joints.push_back(p2);
	color.set(255,255,255,50);
	startAngle = -1.f;
	bFound = true;
}

void ofxSLimb::copy(ofxSLimb * other){
	bReverseOrder = other->bReverseOrder;
	color = other->color;
	joints.clear();
	joints.insert(joints.begin(),other->joints.begin(),other->joints.end());
	line = other->line;
	bFound = true;
}

void ofxSLimb::clear(){
	bFound = false;
	joints.clear();
	line = NULL;
}

ofPoint * ofxSLimb::getLimbStart(){
	if(!bFound)
		return NULL;

	if(bReverseOrder)
		return &joints.back();
	return &joints.front();
}

ofPoint * ofxSLimb::getLimbEnd() {
	if(!bFound)
		return NULL;

	if(bReverseOrder)
		return &joints.front();
	return &joints.back();
}

ofPoint * ofxSLimb::getJoint(int idx) {
	if(idx < 0){
		idx = joints.size() + idx;
	}

	if(!bFound || idx <0 || idx > joints.size()-1)
		return NULL;

	if(bReverseOrder)
		return &joints.at(joints.size()-(1+idx));
	return &joints.at(idx);
}

void ofxSLimb::join(ofPoint & jointP, ofPoint & newEndP, bool bInsertFront){
	if(bInsertFront){
		joints.front() = jointP;//TODO insert intersection (or median)
		joints.insert(joints.begin(),newEndP);
	}else{
		joints.back() = jointP;//TODO insert intersection (or median)
		joints.push_back(newEndP);
	}

}

//float ofxSLimb::getLengthBetween(int startIdx,int endIdx){
//	if(startIdx < 0){
//		startIdx += line->size();
//	}
//	if(endIdx < 0){
//		endIdx += line->size();
//	}
//	if(bReverseOrder){
//		int tmpidx = endIdx;
//		endIdx = line->size()-1-startIdx;
//		startIdx = line->size()-1-tmpidx;
//	}
//}

float ofxSLimb::calcLengthSquared(){
	return calcLength(true);
}

float ofxSLimb::calcLength(bool squared){
	float result = 0;
	for(unsigned int i=0;i<line->size()-1;++i){
		ofPoint & p = line->pixels[i];
		ofPoint & q = line->pixels[i+1];
		if(squared){
			result += p.squareDistance(q);
		}else{
			result += p.distance(q);
		}
	}
	return result;
}

void ofxSLimb::draw(float x, float y){
	ofPushMatrix();
	ofPushStyle();
	ofTranslate(x,y,0);
	ofSetColor(color);
	ofFill();
	ofPoint & p1 = first();
	ofPoint & p2 = last();
	ofEllipse(p1.x,p1.y,10,10);
	ofEllipse(p2.x,p2.y,10,10);
	ofLine(p1.x,p1.y,p2.x,p2.y);

	for(int i=0;i<joints.size()-1;++i){
		ofPoint & p = joints[i];
		ofPoint & q = joints[i+1];
		int dist = round(p.distance(q));
		ofVec3f middle = (p+q)/2.f;
		ofSetColor(255,255,255);
		ofDrawBitmapString(ofToString(dist),middle);
	}

	ofPopMatrix();
	ofPopStyle();
}
