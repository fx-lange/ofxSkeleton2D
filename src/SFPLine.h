/*
 * SFPLine.h
 *
 *  Created on: Jun 18, 2012
 *      Author: spta
 */

#ifndef SFPLINE_H_
#define SFPLINE_H_

#include "ofMain.h"
#include "ofxSFP.h"
#include "ofxSLimb.h"

class SFPLine{
public:
	SFPLine(){
		bMerged = false;
		limb = NULL;
	}
	vector<SFP> pixels;
	bool bMerged;
	SLimb * limb;
	SFP & operator[](int idx){
		return pixels[idx];
	}

	int size(){
		return pixels.size();
	}

	void draw(float x,float y){
		ofTranslate(x,y);
		ofLine(pixels[0],pixels[pixels.size()-1]);
		ofTranslate(-x,-y);
	}

	SFP & first(){
		return pixels[0];
	}

	SFP & last(){
		return pixels[size()-1];
	}

	void add(SFPLine & other,bool bReverse = false, bool bInsertFront = false){
		if(bInsertFront){
			if(bReverse){
				pixels.insert(pixels.begin(),other.pixels.rbegin(),other.pixels.rend());
			}else{
				pixels.insert(pixels.begin(),other.pixels.begin(),other.pixels.end());
			}
		}else{
			if(bReverse){
				pixels.insert(pixels.end(),other.pixels.rbegin(),other.pixels.rend());
			}else{
				pixels.insert(pixels.end(),other.pixels.begin(),other.pixels.end());
			}
		}
		other.bMerged = true;
	}
};

#endif /* SFPLINE_H_ */
