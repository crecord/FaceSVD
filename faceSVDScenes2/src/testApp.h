#pragma once
#include "ofMain.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxXmlSettings.h"
#include "ofxAppUtils.h"

#include "ofxCv.h"

using namespace ofxCv;
using namespace cv;

/* #include <vector> */

class testApp : public ofxApp {
public:

	void printMatrix(cv::Mat &m);
    Float64 cosSim(Float64 &x,Float64 &y,Float64 &x1,Float64 &x2);
    void saveArray(cv::Mat &mat, ofxXmlSettings xml, string name );
    cv::Mat readMatrix(ofxXmlSettings xml, string filename, string matName);
    void svdToXml (string rawData, string svdData);
    ofVec2f findBob(string svdData);
    void highLowMatrixMaker (ofxXmlSettings xmlName ,cv::Mat &m, string variableName, int numFace );
    
    ofxXmlSettings XML;
    int imgCount;
    float color;
	ofVideoGrabber cam;
	ofxFaceTrackerThreaded tracker;
	ExpressionClassifier classifier;
    ofImage mostSimilar;
    string imgPath;
    ofVec2f currentLocation;
    std::vector<ofVec2f> arrayOfCollectedPoints;
    
    // all the app Utils stuff
    
    testApp()  {}
	
    void setup();
    void update();
    void draw();
    
    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    
    void windowResized(int w, int h);
    
    // handles the scenes
    ofxSceneManager sceneManager;
    int lastScene;
};
