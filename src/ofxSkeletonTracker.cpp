/*
 * ofxSkeleton2D.cpp
 *
 *  Created on: Jun 16, 2012
 *      Author: spta
 */

#include "ofxSkeleton2D.h"

int isLeftOf(ofPoint * p1,ofPoint * p2, ofPoint * o){
		float detA = p1->x * p2->y + o->x * p1->y + p2->x * o->y
				- p1->x * o->y - p2->x * p1->y - o->x*p2->y;

		if(detA > 0){
			return -1;
		}else if(detA < 0){
			return 1;
		}else{
			return 0;
		}
}


void ofxSkeletonTracker2D::setup(float _width, float _height) {
	width = _width;
	height = _height;

	fbo.allocate(width, height, GL_RGBA);

	binary.allocate(width, height);

	voronoi.setup(width, height, 1.f);
	voronoi.setSkeletonMode(true);
	voronoi.setPerFeatureVD(true);

	sfpShader.load("shaders/discLocMax.vert", "shaders/discLocMax.frag");

	/* PBO */
	nChannels = 4;
	// generate new PBOs and get the associated IDs
	glGenBuffers(2, pboIds);
	for (int i = 0; i < 2; i++) {
		// bind PBO in order to use
		glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[i]);
		// alloc memory for PBO
		glBufferData(GL_PIXEL_PACK_BUFFER, width * height * nChannels * sizeof(char), 0, GL_STREAM_READ);
	}
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	activeIdx = 0;

	//TORSO
	xss.create(0, 1, CV_32F);
	yss.create(0, 1, CV_32F);

	linePixels.resize(640);

	skeleton.gui = &gui;
}

ofxPanel * ofxSkeletonTracker2D::getGui(){
	return & gui.panel;
}

ofxPanel * ofxSkeletonTracker2D::getImageprocessingPanel(){
	return & gui.imagePanel;
}

ofxPanel * ofxSkeletonTracker2D::getShaderPanel(){
	return & gui.shaderPanel;
}

ofFbo * ofxSkeletonTracker2D::calcSfpAsFbo(ofxCvGrayscaleImage & grayInput, ofxCvGrayscaleImage & background) {
	binary.absDiff(background, grayInput);
	binary.threshold(gui.threshold);
	binary.dilate();
	binary.dilate();
	binary.erode();
	binary.erode();
	return calcSfpAsFbo(binary);
}

ofFbo * ofxSkeletonTracker2D::calcSfpAsFbo(ofxCvGrayscaleImage & binaryInput) {
	contourFinder.findContours(binaryInput, 20, (340 * height) / 3, 10, true); // find holes

	simpleContour.clear();
	if (!contourFinder.blobs.empty()) {
		ofxCvBlob & firstBlob = contourFinder.blobs[0];

		simpleContour.addVertexes(firstBlob.pts);
		simpleContour.simplify(gui.tolerance);

		return calcSfpAsFbo(simpleContour.getVertices());
	} else {
		ofLog(OF_LOG_WARNING, "no silhouette found");
		return NULL;
	}
}

ofFbo * ofxSkeletonTracker2D::calcSfpAsFbo(vector<ofPoint> & silhouette) {
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
	sfpShader.setUniform1f("accuracy", gui.lmpAccuracy);
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

void ofxSkeletonTracker2D::calcSkeleton(){
	GLubyte * ptr = readPixelsToPBO();

	if(!ptr)
		return;

	createSFPGrid(ptr);
	torsoPCA();

	findLines();
	if(gui.bMergeLines){
		mergeLines();
	}

	createLimbsNew();
	locateLimbs();
}

void ofxSkeletonTracker2D::drawBinary(float x, float y){
	binary.draw(x,y);
}

void ofxSkeletonTracker2D::drawVoronoi(float x, float y){
	voronoi.drawFboOnScreen(x,y);
}

void ofxSkeletonTracker2D::drawDebugTorso(float x,float y){
	ofPushMatrix();
	ofTranslate(x,y);
	ofPushStyle();

//	Torso
	ofSetColor(0, 255, 0, 180);
	glBegin(GL_POINTS);
	for (unsigned int i = 0; i < torso.size(); ++i) {
		ofPoint & p = torso[i];
		glVertex2d(p.x, p.y);
	}
	glEnd();

	//Torso PCA
	ofSetColor(255, 255, 255);
	ofEllipse(center, 5, 5);
	ofLine(torsoHigh.x, torsoHigh.y, torsoLow.x, torsoLow.y);
	ofLine(skeleton.upperTorsoFromPCA[0],skeleton.upperTorsoFromPCA[1]);

	ofPopStyle();
	ofPopMatrix();
}

void ofxSkeletonTracker2D::drawDebugLines(float x, float y){
	ofPushMatrix();
	ofTranslate(x,y);
	ofPushStyle();

	//Lines
	ofFill();
	for (int i = 0; i < lines.size(); ++i) {
		ofPoint & p1 = lines[i][0];
		ofPoint & p2 = lines[i][lines[i].size() - 1];
		ofSetColor(255, 255, 0, 100);
		ofEllipse(p1.x, p1.y, 5, 5);
		ofSetColor(0, 255, 255, 100);
		ofEllipse(p2.x, p2.y, 5, 5);
		ofSetColor(0, 0, 255);
		ofLine(p1.x, p1.y, p2.x, p2.y);
	}

	ofPopStyle();
	ofPopMatrix();
}

void ofxSkeletonTracker2D::drawDebugLimbs(float x, float y){
	ofPushMatrix();
	ofTranslate(x,y);
	ofPushStyle();

	//Limbs
	for (int i = 0; i < limbs.size(); ++i) {
		limbs[i]->draw();
		for(int j=0;j<limbs[i]->line->size();++j){
			ofCircle(limbs[i]->line->pixels[j],1);
		}
	}

	ofPopStyle();
	ofPopMatrix();
}

void ofxSkeletonTracker2D::drawDebugSkeleton(float x, float y){
	ofPushMatrix();
	ofTranslate(x,y);
	ofPushStyle();

	//Skeleton
	skeleton.draw();

	ofPopStyle();
	ofPopMatrix();
}

GLubyte * ofxSkeletonTracker2D::readPixelsToPBO() {
	fbo.bind();
	// read pixels from framebuffer to PBO
	// glReadPixels() should return immediately.
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[activeIdx]);
	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0); //read frame t
	glBindBuffer(GL_PIXEL_PACK_BUFFER, 0);
	fbo.unbind();

	activeIdx = 1 - activeIdx;

	// map the PBO to process its data by CPU
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[activeIdx]);
	return (GLubyte*) glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY); //use frame t-1
}

void ofxSkeletonTracker2D::createSFPGrid(GLubyte * pboPtr){
	//clearing
	sfpList.clear();
	torso.clear();
	//safe max depth of last frame
	float lastMaxDepth = maxDepth;
	maxDepth = 0;

	//init openCv stuff
	//TODO clear instead of new?! - TODO is working?
	xss.release();
	yss.release();

	//in doppelter Schleife Ergebnissbild vom LocalMax-Shader durchgehen
	//dabei SFP Grid Datenstruktur aufbauen und Torso Punkte bestimmen (und Torso-BoundingBox [performant O(width+2)] )
	//über den Rot-Kanal werden SFP makiert - im Grünkanal die Tiefe = maximaler Abstand zur Kontour
	int checkPos = 0;
	for (int x = 0; x < width; ++x) {
		linePixels[x].clear();
		for (int y = 0; y < height; ++y) {

			float r = pboPtr[x * nChannels + y * (int) width * nChannels];
			float d = pboPtr[1 + x * nChannels + y * (int) width * nChannels];
			bool bIsTorsoP = false;

			//Torsopunkte finden
			if (d > lastMaxDepth * gui.torsoThreshold) {
				torso.push_back(ofPoint(x, y, d));
				xss.push_back((float) x);
				yss.push_back((float) y);
				bIsTorsoP = true;
			}

			//SFPs finden
			if (r > 0.0) {
				sfpList.push_back(ofPoint(x, y, d));
				ofxSFP p = ofxSFP(x, y, 0);
				p.depth = d;
				p.isTorsoPoint = bIsTorsoP;
				linePixels[x].push_back(p);
				if (d > maxDepth) {
					maxDepth = d;
				}
			}
		}
	}

	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
}

void ofxSkeletonTracker2D::torsoPCA(){
	cv::Mat coords = cv::Mat(0, xss.rows, CV_32F);
	coords.push_back(cv::Mat(xss.t()));
	coords.push_back(cv::Mat(yss.t())); //TODO seems very complicated?! - simplify!

	/** Torso Center */
//	cv::Scalar m = cv::mean(coords); //REVISIT doesnt work - why?
	cv::Scalar mx = cv::mean(xss);
	cv::Scalar my = cv::mean(yss);
	center.set(mx.val[0], my.val[0]);

	/** Torso PCA */
	if (torso.size() > 0) {
		cv::PCA pca = cv::PCA(coords, cv::Mat(), CV_PCA_DATA_AS_COL, 2);
//		cout << "EVector: " << pca.eigenvectors << endl;
//		cout << "EValvues: " << pca.eigenvalues << endl;
		primTorsoDirection.set(pca.eigenvectors.at<float>(0, 0), pca.eigenvectors.at<float>(0, 1));
		float primLength = sqrt(pca.eigenvalues.at<float>(0)) * gui.scaleLambda1;
		torsoHigh = center - primTorsoDirection * primLength;
		torsoLow = center + primTorsoDirection * primLength;

		sndTorsoDirection.set(pca.eigenvectors.at<float>(1, 0), pca.eigenvectors.at<float>(1, 1));
		float sndLength = sqrt(pca.eigenvalues.at<float>(1)) * gui.scaleLambda2;
		skeleton.upperTorsoFromPCA[0] = torsoHigh - sndTorsoDirection * sndLength;
		skeleton.upperTorsoFromPCA[1] = torsoHigh + sndTorsoDirection * sndLength;
	}


	skeleton.torsoCenter = center;
}

vector<ofxSFP*> ofxSkeletonTracker2D::findInit(ofxSFP * active, int manhattenRadius) {
	vector<ofxSFP*> initPoints;
	for (int x = max((int) active->x - manhattenRadius, 0); x <= min(active->x + manhattenRadius, width - 1); ++x) {
		for (unsigned int j = 0; j < linePixels[x].size(); ++j) {
			ofxSFP & nextTry = linePixels[x][j];
			if (gui.bExcludeTorso && nextTry.isTorsoPoint) {
				continue;
			}
			int yDiff = nextTry.y - active->y;
			if (yDiff < -manhattenRadius || nextTry.used) {
				continue;
			} else if (yDiff > manhattenRadius) {
				break;
			} else {
				initPoints.push_back(&nextTry);
			}
		}
	}
	return initPoints;
}
/* findNext(SFP active)
 * in einem kleinen Grid mit active als Mittelpunkt
 * nach eine Nachbarn mit möglichst kleinem Abstand (Manhatten-Metrik) suchen
 */
ofxSFP * ofxSkeletonTracker2D::findNext(ofxSFP * active, int manhattenRadius) {
	int minDiff = manhattenRadius * 5;
	ofxSFP * next = NULL;
	for (int x = max((int) active->x - manhattenRadius, 0); x <= min(active->x + manhattenRadius, width - 1); ++x) {
		int xDiff = x - active->x;
		for (unsigned int j = 0; j < linePixels[x].size(); ++j) {
			ofxSFP & nextTry = linePixels[x][j];
			if (gui.bExcludeTorso && nextTry.isTorsoPoint) {
				continue;
			}
			int yDiff = nextTry.y - active->y;
			if (yDiff < -manhattenRadius || nextTry.used) {
				continue; //REVISIT läuft schritt für schritt in neuen bereich - könnte auch binär gesucht werden
			} else if (yDiff > manhattenRadius) {
				break;
			} else {
//				REVISIT
//				+++x+
//				++ +y
//				+++++
//				y ist nach der Rechnung gleich weit weg obwohl es im grid ein ring weiter außen liegt
//				TODO max(abs(xDiff),abs(yDiff)) < minDiff
				int tmpDiff = abs(xDiff) + abs(yDiff);
				if (tmpDiff < minDiff) {
					next = &nextTry;
					minDiff = tmpDiff;
				}
			}
		}
	}
	return next;
	//TODO auf dem weg ++x könnte minDiff schon < +manhattenRadius sein - früher abbrechen
}

//TODO wieso wird hier der "beste" gefunden?
/*
 * es wird sich von links nach rechts und oben nach unten durchs grid bewegt
 * TODO minDiff wird aber nicht aktualisiert
 * REVISIT ist denn der nächste der beste?!
 * Das gewünschte habe ich wohl schon in findNext umgesetzt - welches wird denn benutzt? findBest!
 */
ofxSFP * ofxSkeletonTracker2D::findBest(ofxSFP * last, ofxSFP * active, int manhattenRadius) {
	ofxSFP * next = NULL;
	int maxDist = 0;
	vector<ofxSFP*> beatenNeighbors;
	for (int x = max((int) active->x - manhattenRadius, 0); x <= min(active->x + manhattenRadius, width - 1); ++x) {
		for (unsigned int j = 0; j < linePixels[x].size(); ++j) {
			ofxSFP & nextTry = linePixels[x][j];
			if (gui.bExcludeTorso && nextTry.isTorsoPoint) {
				continue;
			}
			int xDiff = nextTry.x - active->x;
			int yDiff = nextTry.y - active->y;
			if (yDiff < -manhattenRadius || nextTry.used) {
				continue; //REVISIT läuft schritt für schritt in neuen bereich - könnte auch binär gesucht werden
			} else if (yDiff > manhattenRadius) {
				break;
			} else {
//				if (max(abs(xDiff), abs(yDiff)) < manhattenRadius) {//REVISIT checked twice
				int dist = max(abs(xDiff),abs(yDiff));
				if(dist >= maxDist){
					if(next!=NULL)
						beatenNeighbors.push_back(next);
					next = &nextTry;
					maxDist = dist;
				}else{
					beatenNeighbors.push_back(&nextTry);
				}
				nextTry.used = true;
			}
		}
	}
	if(next!=NULL)
		next->beatenNeighbors = beatenNeighbors;
	return next;
	//TODO auf dem weg ++x könnte minDiff schon < +manhattenRadius sein - früher abbrechen
}

void ofxSkeletonTracker2D::findLines() {
	lines.clear();

	//doppelte for-schleife um grid von SFPs zu durchlaufen
	//jeder unbenutzte SFP wird als möglicher startpunkt einer linie betrachtet
	//REVISIT alternative könnte geziehlt von inner nach außen oder umgekehrt gesucht werden
	//	geometry shader könnten zB. gut nach endpunkten suchen?!
	for (int x = 0; x < width; ++x) {
		for (unsigned int i = 0; i < linePixels[x].size(); ++i) {

			ofxSFP & start = linePixels[x][i];
			if (start.used || (gui.bExcludeTorso && start.isTorsoPoint)) {
				continue;
			}
			ofxSFP * last = &start;
			last->used = true;
			ofxSFP * active;
			bool bFound = false;

			/*liste an nachbarn zum startpunkt erstellen -
			 in richtung jeden nachbars versuchen eine linie zu entwickeln
			 abbruch wenn eine linie welche lang genug ist gefunden wurde
			 TODO alternativ könnten mehrere entwickelt werden (nicht nur eine)
			 oder die längste genommen werden (statt der ersten guten)
			 */
			vector<ofxSFP*> initPoints;
			if(gui.bUseFindInit){
				initPoints = findInit(last, gui.manhattenRadius);
			}else{
				if(gui.bFindBest){
					initPoints.push_back(findBest(NULL,last,gui.manhattenRadius));
				}else{
					initPoints.push_back(findNext(last,gui.manhattenRadius));
				}
			}
			for (unsigned int j = 0; j < initPoints.size() && !bFound; ++j) {
//				active = findNext(last, manhattenRadius);
				active = initPoints[j];
				if (active != NULL) {

					//Linie erstellen und erste Punkte hinzufügen
					ofxSFPLine sfpLine; //REVISIT nicht umbedingt nötig eine ganze Linie zu speichern - Am Ende würden 2 Punkte reichen - oder kann man sie gebrauchen?
					sfpLine.pixels.push_back(*last);
					sfpLine.pixels.push_back(*active);
					active->used = true; //REVISIT wieso wurde start bzw last noch nicht auf used=true gesetzt

					//nach nächsten Punkt suchen mit findNext(SFP aktiv)
					ofxSFP * next;
					if (gui.bFindBest) {
						next = findBest(NULL, active, gui.manhattenRadius);
					} else {
						next = findNext(active, gui.manhattenRadius);
					}

					//in Schleife: 1. Winkel zu neuem Punkt prüfen (zwischen first-aktiv und aktiv-next)
					//			   2. wenn Kriterien erfüllt Punkt hinzufügen und nach neuem Punkt suchen
					int chances = 1;
					while (next != NULL) {
						ofVec3f v0;
//						if (gui.bLineMethodA) {
//							v0 = *active - *last;
//						} else {
							v0 = *active - start;
//						}
						ofVec3f v1 = *next - *active;
						float angle = abs(v0.angle(v1));
						float easyAngle = gui.maxAngle;

						//EASY START - Winkelkriterium für noch kurze Linien lockern
						/* Beispiel Faktor = 5 maxAngle = 40
						 * line.size = 2 => easyAngle = 100
						 * line.size = 3 => easyAngle = 66.6
						 * line.size = 4 => easyAngle = 50
						 * line.size = 5 => easyAngle = 40
						 * line.size = 6 => easyAngle = 40 ...
						 */

						if (gui.bEasyStartAngle) {
							easyAngle = max((float) gui.easyStartFaktor / (float) sfpLine.size(), 1.f) * gui.maxAngle;
						}

						//wenn Winkel < tmpAngle => Punkt hinzufügen und nach neuem suchen! mit findNext(SFP aktiv)
						if (angle <= easyAngle) {
							last = active;
							active = next;
							sfpLine.pixels.push_back(*active);
							active->used = true;
							if (gui.bFindBest) {
								next = findBest(last, active, gui.manhattenRadius);
							} else {
								next = findNext(active, gui.manhattenRadius);
							}
						} else {
							if(gui.bFindBest && !gui.bSecondChance && gui.manhattenRadius / ++chances > 1){
								next->freeBeatenNeighbors();
								//try smaller radius
								next = findBest(last, active, gui.manhattenRadius / chances);
							}

							/* SECOND CHANCE - Ausreißer mit schlechten Winkeln überspringen und Nachfolger prüfen
							 * wenn Winkel zu groß wird der Punkt trotzdem hinzugefügt - temporär
							 * es wird dann in C Versuchen die Linie weitergebildet
							 * in der Hoffnung, dass die Winkel wieder kleiner werden
							 */
							else if (gui.bSecondChance && chances < gui.maxChances) {
								next->used = true;
								sfpLine.pixels.push_back(*next);
								next = findNext(next, gui.manhattenRadius); //TODO wieso hier immer findNext?!
								++chances;
							} else {
								next = NULL;
								//TODO popback last chances
							}
						}
					}
					//Abbruch wenn kein neuer Nachbar gefunden wird oder Chancen verbraucht sind
					//Linie wird nur übernommen wenn sie lang genug ist
					if (sfpLine.pixels.size() >= gui.minCountPixelsPerLine) {
						lines.push_back(sfpLine);
						bFound = true;
					} else {
						for (unsigned int j = 0; j < sfpLine.pixels.size(); ++j) {
							sfpLine.pixels[j].used = false;
							sfpLine.pixels[j].freeBeatenNeighbors();
						}
					}
				}
			}
		}
	}
}

/* mergeLines()
 * Linien von denen Endpunkte nah genug aneinander liegen können verschmolzen werden
 * Winkelkriterium muss geprüft werden
 * es müssen pro Linienpaar 4 Fälle untersucht werden
 * REVISIT wenn das Abstands- aber nicht das Winkelkriterium  erfüllt wird
 * 		handelt es sich um ein Gelenk - möglicher Performancegewinn
 * 		man könnte mergeLines() und createLimbs() zusammenfassen indem man
 * 		SFPLine und SLimb in einer Klasse zusammenfasst
 */
void ofxSkeletonTracker2D::mergeLines() {
	for (unsigned int i = 0; i < lines.size(); ++i) {
		ofxSFPLine & line = lines[i];
		if (line.bMerged) {
			continue;
		}

		for (unsigned int j = 0; j < lines.size(); ++j) { //TODO j könnte auch bei i anfangen?!
			ofxSFPLine & other = lines[j];
			if (i == j || other.bMerged) {
				continue;
			}

			int mergeCase = 0;
			float minDistance = gui.maxMergeDistance;
			//TODO use a tmpDistance
			float tmpDistance = line.last().distance(other.first());
			if (tmpDistance < minDistance) {
				mergeCase = 1;
				minDistance = tmpDistance;
			}

			tmpDistance = line.last().distance(other.last());
			if (tmpDistance < minDistance) {
				mergeCase = 2;
				minDistance = tmpDistance;
			}

			tmpDistance = line.first().distance(other.last());
			if (tmpDistance < minDistance) {
				mergeCase = 3;
				minDistance = tmpDistance;
			}

			tmpDistance = line.first().distance(other.first());
			if (tmpDistance < minDistance) {
				mergeCase = 4;
			}

			if(mergeCase == 1){
				ofVec2f v1 = line.last() - line.first();
				ofVec2f v2 = other.last() - other.first();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, false);
				} else {
					//Aus Linien Gliedmaßen bilden
					//Fälle Line ist schon ein Glied
				}
			}else if(mergeCase == 2){
				ofVec2f v1 = line.last() - line.first();
				ofVec2f v2 = other.first() - other.last();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, true);
				}
			}else if(mergeCase == 3){
				ofVec2f v1 = line.first() - line.last();
				ofVec2f v2 = other.first() - other.last();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, false, true);
				}
			}else if(mergeCase == 4){
				ofVec2f v1 = line.first() - line.last();
				ofVec2f v2 = other.last() - other.first();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, true, true);
				}
			}
		}
	}
	//übrige (verschmolzende) Linen löschen
	vector<ofxSFPLine>::iterator it = lines.begin();
	for (; it != lines.end();) {
		if (it->bMerged) {
			it = lines.erase(it);
		} else {
			++it;
		}
	}
}

//void ofxSkeletonTracker2D::createLimbs() {
//	float squaredMinLimbLength = gui.minlineLength*gui.minlineLength;
//	limbs.clear(); //TODO memory leak
//	for (int i = 0; i < lines.size(); ++i) {
//		ofxSFPLine & line = lines[i];
//		ofxSLimb * limb = NULL;
//		if (line.limb != NULL || line.lengthSquared() < squaredMinLimbLength) {
//			continue;
//		} else {
//			line.limb = limb = new ofxSLimb(line.first(), line.last());
//		}
//
//		for (int j = 0; j < lines.size(); ++j) {
//
//			//TODO i != j
//			ofxSFPLine & other = lines[j];
//			if (other.limb != NULL || other.lengthSquared() < squaredMinLimbLength) {
//				continue;
//			}
//			if (limb->last().distance(other.first()) < gui.maxLimbPointDistance) {
//				limb->join(other.first(), other.last(), false);
//				other.limb = limb;
//			} else if (limb->last().distance(other.last()) < gui.maxLimbPointDistance) {
//				limb->join(other.last(), other.first(), false);
//				other.limb = limb;
//			} else if (limb->first().distance(other.last()) < gui.maxLimbPointDistance) {
//				limb->join(other.last(), other.first(), true);
//				other.limb = limb;
//			} else if (limb->first().distance(other.first()) < gui.maxLimbPointDistance) {
//				limb->join(other.first(), other.last(), true);
//				other.limb = limb;
//			}
//		}
//		limbs.push_back(limb);
//	}
//}

void ofxSkeletonTracker2D::createLimbsNew() {
	float squaredMinLimbLength = gui.minlineLength*gui.minlineLength;
	limbs.clear(); //TODO memory leak
	for (int i = 0; i < lines.size(); ++i) {
		ofxSFPLine & line = lines[i];
		ofxSLimb * limb = NULL;
		if (line.lengthSquared() < squaredMinLimbLength)
			continue;

		limb = new ofxSLimb(line.first(), line.last());
		limb->line = &line;

		limbs.push_back(limb);
	}
}

void ofxSkeletonTracker2D::locateLimbs() {
	//TODO wirkt sehr aufwenig um die 3 nächsten zu suchen.

	vector<ofxSLimb*> nearest;
	vector<float> minDistances;
	minDistances.push_back(20000);
	minDistances.push_back(20000);
	minDistances.push_back(20000);
	minDistances.push_back(20000);
	minDistances.push_back(20000);
	nearest.push_back(NULL);
	nearest.push_back(NULL);
	nearest.push_back(NULL);
	nearest.push_back(NULL);
	nearest.push_back(NULL);


	//sort SLimbs after their smallest distance to torsoHigh
	for (int i = 0; i < limbs.size(); ++i) {
		ofxSLimb * limb = limbs[i];
		float distanceToCheck = 0;
		float distanceFirst = limb->first().distance(torsoHigh);
		float distanceLast = limb->last().distance(torsoHigh);
		if (distanceFirst < distanceLast) {
			distanceToCheck = distanceFirst;
			limb->bReverseOrder = false;
		} else {
			distanceToCheck = distanceLast;
			limb->bReverseOrder = true;
		}
		for (int j = 0; j < 5; ++j) {
			if (distanceToCheck < minDistances[j]) {
				for (int k = 4; k > j; k--) {
					minDistances[k] = minDistances[k - 1];
					nearest[k] = nearest[k - 1];
				}
				minDistances[j] = distanceToCheck;
				nearest[j] = limb;
				break;
			}
		}
	}

	vector<ofxSLimb*> minAngleLimbs;
	vector<float> minAngles;
	minAngles.push_back(20000);
	minAngles.push_back(20000);
	minAngles.push_back(20000);
	minAngles.push_back(20000);
	minAngles.push_back(20000);
	minAngleLimbs.push_back(NULL);
	minAngleLimbs.push_back(NULL);
	minAngleLimbs.push_back(NULL);
	minAngleLimbs.push_back(NULL);
	minAngleLimbs.push_back(NULL);


	//Pose Estimation - Head vs Arms
	//a) v2 = torsoLow_torsoHigh v1 = torsoHigh_limbStart -> dcVcb
	//b) v2 = torsoLow_torsoHigh v1 = limbStart_limbEnd -> dcVba
	// a) is better than b)

	//sort SLimbs after their smallest angle to the primary component of the torso
	ofVec2f v2 = center - torsoLow; //use center instead of torsoHigh - same angle but no bug if torso high is higher than neck
	float maxAngle = 150.f;
	float squaredMinLimbLength = gui.minLimbLengthSum * gui.minLimbLengthSum;
	for (int i = 0; i < limbs.size(); ++i) {
		ofxSLimb * limb = limbs[i];
		if(limb->getLengthSquared() < squaredMinLimbLength){
			continue;
		}
		ofPoint * sJoint = limb->getLimbStart();
		ofVec2f v1 = *sJoint - center;
		float tmpAngle = abs(v1.angle(v2));
		limb->startAngle = tmpAngle;
		for (int j = 0; j < 5; ++j) {
			if (tmpAngle < minAngles[j]) {
				for (int k = 4; k > j; k--) {
					minAngles[k] = minAngles[k - 1];
					minAngleLimbs[k] = minAngleLimbs[k - 1];
				}
				minAngles[j] = tmpAngle;
				minAngleLimbs[j] = limb;
				break;
			}
		}
	}

	int nextIdx = 0;
	skeleton.resetFrame();
	if (minAngleLimbs[nextIdx] != NULL) {

		if(minAngles[nextIdx] <= gui.maxAngleHead){
			skeleton.neckToHead.copy(minAngleLimbs[nextIdx]);
			++nextIdx;
		}

		//arms
//		if (minAngleLimbs[nextIdx] != NULL && minAngleLimbs[nextIdx]->startAngle < maxAngle) {
		if (minAngleLimbs[nextIdx] != NULL && minAngleLimbs[nextIdx]->getLimbStart()->y < torsoLow.y) {
			skeleton.arms[0].copy(minAngleLimbs[nextIdx]);
			++nextIdx;
//			if (minAngleLimbs[nextIdx] != NULL && minAngleLimbs[nextIdx]->startAngle < maxAngle) {
			if (minAngleLimbs[nextIdx] != NULL && minAngleLimbs[nextIdx]->getLimbStart()->y < torsoLow.y) {
				skeleton.arms[1].copy(minAngleLimbs[nextIdx]);
				++nextIdx;
			}
		}

		if(nextIdx >= 3){//check if head is between shoulders ...
//			if(skeleton.neckToHead.isLeftOf(skeleton.arms[0]) == skeleton.neckToHead.isLeftOf(skeleton.arms[1])){
//				cout << "HEAD IS NOT BETWEEN SHOULDERS!" << endl;
//			}
			if(isLeftOf(&center,skeleton.neckToHead.getLimbStart(),skeleton.arms[0].getLimbStart()) == isLeftOf(&center,skeleton.neckToHead.getLimbStart(),skeleton.arms[1].getLimbStart())){
				cout << "HEAD IS NOT BETWEEN SHOULDERS!" << endl;
				ofxSLimb tmp;
				tmp.copy(&skeleton.neckToHead);
				skeleton.neckToHead.copy(&skeleton.arms[0]);
				skeleton.arms[0].copy(&tmp);
			}
		}

		//TODO!!!!
		//legs
		if (minAngleLimbs[nextIdx] != NULL && minAngleLimbs[nextIdx]->startAngle > maxAngle) {
			skeleton.legs[0].copy(minAngleLimbs[nextIdx]);
			if (minAngleLimbs[nextIdx + 1] != NULL && minAngleLimbs[nextIdx + 1]->startAngle > maxAngle) {
				skeleton.legs[1].copy(minAngleLimbs[nextIdx + 1]);
			}
		}
	}

	skeleton.update();

//	//Unterscheidung von Kopf und Fuß über Ähnlichkeit der Winkel zum Torso
//	//TODO Gerade Arme oder sehr schiefer Kopf könnten hier Fehler verursachen
//	float minAngle = 360.f;
//	int minIndex = 0;
//	ofVec2f v2 = torsoHigh - torsoLow;
//	for(int i=0;i<3;i++){
//		if(nearest[i] == NULL){
//			break;
//		}
//
////		ofVec2f v1 = nearest[i]->getLimbEnd() - nearest[i]->getLimbStart();
//		ofVec2f v1 = nearest[i]->getLimbStart() - torsoHigh;
//		float tmpAngle = abs(v1.angle(v2));
//		if(tmpAngle < maxAngle && tmpAngle < minAngle){
//			minIndex = i;
//			minAngle = tmpAngle;
//		}
//	}
//
//	if(nearest[minIndex] != NULL){
//		skeleton.head = nearest[minIndex];
//		if(nearest[(minIndex+1)%3] != NULL){
//			skeleton.arms[0] = nearest[(minIndex+1)%3];
//			if(nearest[(minIndex+2)%3] != NULL){
//				skeleton.arms[1] = nearest[(minIndex+2)%3];
//			}
//		}
//	}
}

// ================================================================================
