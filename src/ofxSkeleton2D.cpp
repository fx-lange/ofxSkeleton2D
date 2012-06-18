/*
 * ofxSkeleton2D.cpp
 *
 *  Created on: Jun 16, 2012
 *      Author: spta
 */

#include "ofxSkeleton2D.h"

void ofxSkeleton2D::setup(float _width, float _height) {
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
	torsoThreshold = 0.7;

	//TORSO
	xss.create(0, 1, CV_32F);
	yss.create(0, 1, CV_32F);

	linePixels.resize(640);
}

ofxPanel * ofxSkeleton2D::getGui(){
	return & gui.panel;
}

ofFbo * ofxSkeleton2D::calcSfpAsFbo(ofxCvGrayscaleImage & grayInput, ofxCvGrayscaleImage & background, int threshold, float simplifyTolerance) {
	binary.absDiff(background, grayInput);
	binary.threshold(threshold);
	return calcSfpAsFbo(binary, simplifyTolerance);
}

ofFbo * ofxSkeleton2D::calcSfpAsFbo(ofxCvGrayscaleImage & binaryInput, float simplifyTolerance) {
	contourFinder.findContours(binaryInput, 20, (340 * height) / 3, 10, true); // find holes

	simpleContour.clear();
	if (!contourFinder.blobs.empty()) {
		ofxCvBlob & firstBlob = contourFinder.blobs[0];

		simpleContour.addVertexes(firstBlob.pts);
		simpleContour.simplify(simplifyTolerance);

		return calcSfpAsFbo(simpleContour.getVertices());
	} else {
		ofLog(OF_LOG_WARNING, "no silhouette found");
		return NULL;
	}
}

ofFbo * ofxSkeleton2D::calcSfpAsFbo(vector<ofPoint> & silhouette) {
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

void ofxSkeleton2D::constructLines(){
	GLubyte * ptr = readPixelsToPBO();

	if(!ptr)
		return;

	createSFPGrid(ptr);
	torsoPCA();

	findLines();
	if(gui.bMergeLines){
		mergeLines();
	}
}

void ofxSkeleton2D::drawBinary(float x, float y){
	binary.draw(x,y);
}

void ofxSkeleton2D::drawDebugTorso(float x,float y){
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

	//Torso BB
	ofSetColor(255, 255, 255, 180);
	ofNoFill();
	ofRect(bbTorso);

	ofPopStyle();
	ofPopMatrix();
}

void ofxSkeleton2D::drawDebugLines(float x, float y){
	ofPushMatrix();
	ofTranslate(x,y);
	ofPushStyle();

	//Lines
	ofFill();
	for (int i = 0; i < lines.size(); ++i) {
		ofPoint & p1 = lines[i][0];
		ofPoint & p2 = lines[i][lines[i].size() - 1];
		ofSetColor(255, 255, 0, 100);
		ofEllipse(p1.x, p1.y, 15, 15);
		ofSetColor(0, 255, 255, 100);
		ofEllipse(p2.x, p2.y, 15, 15);
		ofSetColor(0, 0, 255);
		ofLine(p1.x, p1.y, p2.x, p2.y);
	}

	ofPopStyle();
	ofPopMatrix();
}

GLubyte * ofxSkeleton2D::readPixelsToPBO() {
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

void ofxSkeleton2D::createSFPGrid(GLubyte * pboPtr){
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

	ofPoint bb1, bb2;
	bb1.set(640, 480, 0);
	bb2.set(0, 0, 0);

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
			if (d > lastMaxDepth * torsoThreshold) {
				torso.push_back(ofPoint(x, y, d));
				xss.push_back((float) x);
				yss.push_back((float) y);
				bIsTorsoP = true;
			}

			//SFPs finden
			if (r > 0.0) {
				sfpList.push_back(ofPoint(x, y, d));
				SFP p = SFP(x, y, d);
				p.isTorsoPoint = bIsTorsoP;
				linePixels[x].push_back(p);
				if (d > maxDepth) {
					maxDepth = d;
				}
			}
		}
		//calc ymin and ymax of boundingbox
		if (yss.rows > checkPos) {
			float y = yss.at<float>(checkPos);
			bb1.y = y < bb1.y ? y : bb1.y;
			y = yss.at<float>(yss.rows - 1);
			bb2.y = y > bb2.y ? y : bb2.y;
			checkPos = yss.rows;
		}
	}
	//calc xmin and xmax of boundingbox
	if (xss.rows > 0) {
		bb1.x = xss.at<float>(0);
		bb2.x = xss.at<float>(xss.rows - 1);
		bbTorso.set(bb1, bb2.x - bb1.x, bb2.y - bb1.y);
	}

	glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
}

void ofxSkeleton2D::torsoPCA(){
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
	}

	float length = abs(bbTorso.y + (bbTorso.height * 0.125f) - center.y);
	torsoHigh = center - primTorsoDirection * length;
	length = abs(bbTorso.y + (bbTorso.height * (1 - 0.125f)) - center.y);
	torsoLow = center + primTorsoDirection * length;
}

vector<SFP*> ofxSkeleton2D::findInit(SFP * active, int manhattenRadius) {
	vector<SFP*> initPoints;
	for (int x = max((int) active->x - manhattenRadius, 0); x <= min(active->x + manhattenRadius, width - 1); ++x) {
		for (unsigned int j = 0; j < linePixels[x].size(); ++j) {
			SFP & nextTry = linePixels[x][j];
			if (gui.bExcludeTorso && nextTry.isTorsoPoint) {
				continue;
			}
			int yDiff = nextTry.y - active->y;
			if (yDiff < -manhattenRadius || nextTry.used) {
				continue;
			} else if (yDiff > manhattenRadius) {
				break;
			} else {
//					int tmpDiff = abs(xDiff)+abs(yDiff);
//					if(tmpDiff<minDiff){
//						next = &nextTry;
//						minDiff = tmpDiff;
//					}
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
SFP * ofxSkeleton2D::findNext(SFP * active, int manhattenRadius) {
	int minDiff = manhattenRadius * 5;
	SFP * next = NULL;
	for (int x = max((int) active->x - manhattenRadius, 0); x <= min(active->x + manhattenRadius, width - 1); ++x) {
		int xDiff = x - active->x;
		for (unsigned int j = 0; j < linePixels[x].size(); ++j) {
			SFP & nextTry = linePixels[x][j];
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
 * Das gewünschte habe ich wohl schon in findNext umgesetzt - welches wird denn benutzt? findNext!
 */
SFP * ofxSkeleton2D::findBest(SFP * last, SFP * active, int manhattenRadius) {
	int minDiff = manhattenRadius * 2;
	SFP * next = NULL;
	for (int x = max((int) active->x - manhattenRadius, 0); x <= min(active->x + manhattenRadius, width - 1); ++x) {
		int xDiff = x - active->x;
		for (unsigned int j = 0; j < linePixels[x].size(); ++j) {
			SFP & nextTry = linePixels[x][j];
			if (gui.bExcludeTorso && nextTry.isTorsoPoint) {
				continue;
			}
			int yDiff = nextTry.y - active->y;
			if (yDiff < -manhattenRadius || nextTry.used) {
				continue; //REVISIT läuft schritt für schritt in neuen bereich - könnte auch binär gesucht werden
			} else if (yDiff > manhattenRadius) {
				break;
			} else {
				if (max(abs(xDiff), abs(yDiff)) < minDiff) {
					next = &nextTry;
				}
				nextTry.used = true;
			}
		}
	}
	return next;
	//TODO auf dem weg ++x könnte minDiff schon < +manhattenRadius sein - früher abbrechen
}

void ofxSkeleton2D::findLines() {
	lines.clear();

	//doppelte for-schleife um grid von SFPs zu durchlaufen
	//jeder unbenutzte SFP wird als möglicher startpunkt einer linie betrachtet
	//REVISIT alternative könnte geziehlt von inner nach außen oder umgekehrt gesucht werden
	//	geometry shader könnten zB. gut nach endpunkten suchen?!
	for (int x = 0; x < width; ++x) {
		for (unsigned int i = 0; i < linePixels[x].size(); ++i) {

			SFP & start = linePixels[x][i];
			if (start.used || (gui.bExcludeTorso && start.isTorsoPoint)) {
				continue;
			}
			SFP * last = &start;
			SFP * active;
			bool bFound = false;

			/*liste an nachbarn zum startpunkt erstellen -
			 in richtung jeden nachbars versuchen eine linie zu entwickeln
			 abbruch wenn eine linie welche lang genug ist gefunden wurde
			 TODO alternativ könnten mehrere entwickelt werden (nicht nur eine)
			 oder die längste genommen werden (statt der ersten guten)
			 */
			vector<SFP*> initPoints = findInit(last, gui.manhattenRadius);
			for (unsigned int j = 0; j < initPoints.size() && !bFound; ++j) {
//				active = findNext(last, manhattenRadius);
				active = initPoints[j];
				if (active != NULL) {

					//Linie erstellen und erste Punkte hinzufügen
					SFPLine sfpLine; //REVISIT nicht umbedingt nötig eine ganze Linie zu speichern - Am Ende würden 2 Punkte reichen - oder kann man sie gebrauchen?
					sfpLine.pixels.push_back(*last);
					sfpLine.pixels.push_back(*active);
					active->used = true; //REVISIT wieso wurde start bzw last noch nicht auf used=true gesetzt

					//nach nächsten Punkt suchen mit findNext(SFP aktiv)
					SFP * next;
					if (gui.bFindBest) {
						next = findBest(last, active, gui.manhattenRadius);
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
							/* SECOND CHANCE - Ausreißer mit schlechten Winkeln überspringen und Nachfolger prüfen
							 * wenn Winkel zu groß wird der Punkt trotzdem hinzugefügt - temporär
							 * es wird dann in C Versuchen die Linie weitergebildet
							 * in der Hoffnung, dass die Winkel wieder kleiner werden
							 */
							if (gui.bSecondChance && chances < gui.maxChances) {
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
					if (sfpLine.pixels.size() > gui.minCountPixelsPerLine) {
						lines.push_back(sfpLine);
						bFound = true;
					} else {
						for (unsigned int j = 0; j < sfpLine.pixels.size(); ++j) {
							sfpLine.pixels[j].used = false;
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
void ofxSkeleton2D::mergeLines() {
	for (unsigned int i = 0; i < lines.size(); ++i) {
		SFPLine & line = lines[i];
		if (line.bMerged) {
			continue;
		}

		for (unsigned int j = 0; j < lines.size(); ++j) { //TODO j könnte auch bei i anfangen?!
			SFPLine & other = lines[j];
			if (i == j || other.bMerged) {
				continue;
			}
			if (line.last().distance(other.first()) < gui.maxMergeDistance) {
				ofVec2f v1 = line.last() - line.first();
				ofVec2f v2 = other.last() - other.first();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, false);
				} else {
					//Aus Linien Gliedmaßen bilden
					//Fälle Line ist schon ein Glied
				}
			} else if (line.last().distance(other.last()) < gui.maxMergeDistance) {
				ofVec2f v1 = line.last() - line.first();
				ofVec2f v2 = other.first() - other.last();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, true);
				}
			} else if (line.first().distance(other.last()) < gui.maxMergeDistance) {
				ofVec2f v1 = line.first() - line.last();
				ofVec2f v2 = other.first() - other.last();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, false, true);
				}
			} else if (line.first().distance(other.first()) < gui.maxMergeDistance) {
				ofVec2f v1 = line.first() - line.last();
				ofVec2f v2 = other.last() - other.first();
				if (abs(v1.angle(v2)) < gui.maxMergeAngle) {
					line.add(other, true, true);
				}
			}
		}
	}
	//übrige (verschmolzende) Linen löschen
	vector<SFPLine>::iterator it = lines.begin();
	for (; it != lines.end();) {
		if (it->bMerged) {
			it = lines.erase(it);
		} else {
			++it;
		}
	}
}

// ================================================================================
