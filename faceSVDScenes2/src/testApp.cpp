#include "testApp.h"
#include "scenes/scenes.h"



//--------------------------------------------------------------
void testApp::setup() {
    
	// setup for nice jaggy-less rendering
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground(0, 0, 0);
    cam.initGrabber(640, 480);
    tracker.setup();
    tracker.setRescale(.5);
    imgCount = 0;
    
	// setup the render size (working area)
	setRenderSize(640, 480);
    
	// turn on transform origin translation and scaling to screen size,
	// disable quad warping, and enable aspect ratio and centering when scaling
	setTransforms(true, true, false, false, true);
    
	// the control panel is setup automatically, of course you can still change
	// all the settings manually here
    
	// add the built in transform control to control panel (adds new panel)
    //
    // loads and saves control panel settings to "controlPanelSettings.xml"
    // in the data folder
	addTransformControls();
	
	// load saved control panel settings
	// loads and saves to "controlPanelSettings.xml" in the data folder
	// or use your own filename
	loadControlSettings();
    
    // load saved quad warper settings
    // loads and saves to "quadWarper.xml" in the data folder
	// or use your own filename
    loadWarpSettings();
	
	// load scenes
	sceneManager.add(new collectionPortraite());
	sceneManager.add(new portraitePuppet());
    //sceneManager.add(new collectionManyFace());
    //sceneManager.add(new ManyFacePuppet());
	sceneManager.setup(true);	// true = setup all the scenes now (not on the fly)
	ofSetLogLevel("ofxSceneManager", OF_LOG_VERBOSE); // lets see whats going on inside
	
	// start with a specific scene
	// set now to true in order to ignore the scene fade and change now
	sceneManager.gotoScene("portraitePuppet", true);
	lastScene = sceneManager.getCurrentSceneIndex();
	
	// attach scene manager to this ofxApp so it's called automatically,
	// you can also call the callbacks (update, draw, keyPressed, etc) manually
    // if you don't set it
	//
	// you can also turn off the auto sceneManager update and draw calls with:
	// setSceneManagerUpdate(false);
	// setSceneManagerDraw(false);
	//
	// the input callbacks in your scenes will be called if they are implemented
	//
	setSceneManager(&sceneManager);
}

//--------------------------------------------------------------
void testApp::update() {
    cam.update();
    if(cam.isFrameNew()) {
        if(tracker.update(toCv(cam))) {
            classifier.classify(tracker);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw() {
    //cam.draw(0, 0);
	
    classifier.load("expressions");
	
	// drop out of the auto transform space back to OF screen space
	popTransforms();
	
	// draw current scene info using the ofxBitmapString stream interface
	// to ofDrawBitmapString
    if(bDebug) {
        ofSetColor(200);
        ofxBitmapString(12, ofGetHeight()-8)
        << "Current Scene: " << sceneManager.getCurrentSceneIndex()
        << " " << sceneManager.getCurrentSceneName() << endl;
    }

    // ofGet
	// go back to the auto transform space
	//
	// this is actually done automatically if the transforms were popped
	// before the control panel is drawn, but included here for completeness
	pushTransforms();
    
	// the control panel and warp editor are drawn automatically after this
	// function
}

// current scene input functions are called automatically before calling these
//--------------------------------------------------------------
void testApp::keyPressed(int key) {
	
	switch (key) {
            
		case 'd':
			bDebug = !bDebug;
			break;
			
		case 'a':
			setAspect(!getAspect());
			break;
			
		case 'c':
			setCentering(!getCentering());
			break;
			
		case 'm':
			setMirrorX(!getMirrorX());
			break;
			
		case 'n':
			setMirrorY(!getMirrorY());
			break;
			
		case 'q':
			setWarp(!getWarp());
			break;
			
		case 'f':
			ofToggleFullscreen();
			break;
            
		case OF_KEY_LEFT:
            ofLog(OF_LOG_NOTICE, "move Left");
			sceneManager.prevScene(true);
			break;
			
		case OF_KEY_RIGHT:
            ofLog(OF_LOG_NOTICE, "move Right");
			sceneManager.nextScene(true);
			break;
			
		case OF_KEY_DOWN:
			if(sceneManager.getCurrentScene()) { // returns NULL if no scene selected
				lastScene = sceneManager.getCurrentSceneIndex();
			}
			sceneManager.noScene(true);
			break;
			
		case OF_KEY_UP:
			sceneManager.gotoScene(lastScene);
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {
    
	// set up transforms with new screen size
	setNewScreenSize(w,h);
}

void testApp::saveArray (cv::Mat &mat, ofxXmlSettings xml, string name ){
    ofLog(OF_LOG_NOTICE, "saveArray received the following matrix");
    printMatrix(mat);
    xml.addTag(name);
    xml.pushTag(name);
    for( int row=0; row<mat.rows; row++){
        xml.addTag("row");
        xml.pushTag("row", row);
        // get the entire row of mat
        const Float64* wholeRow = mat.ptr<Float64>(row);
        for(int col = 0; col < mat.cols; col++) {
            Float64 currentNum = wholeRow[col];
            string index = ofToString(col);
            xml.addValue("num",currentNum);
        }
        xml.popTag();
    }
    xml.popTag();
}

void testApp::printMatrix(cv::Mat &m){
    
    string rowString = "";
    
    for(int i = 0; i < m.rows; i++) {
        const Float64* Mi = m.ptr<Float64>(i);
        for(int j = 0; j < m.cols; j++) {
            rowString += ofToString(Mi[j]) + " ";
        }
        ofLog(OF_LOG_NOTICE, rowString);
        rowString = "";
    }
}

Float64 testApp::cosSim(Float64 &x,Float64 &y,Float64 &x1,Float64 &y1 ){
    Float64 dotty  = x*x1 + y*y1;
    // printf("dotty: %F",dotty);
    Float64 d1 = sqrt(pow(x,2)+pow(y,2));
    // printf("d1: %F", d1);
    Float64 d2 = sqrt(pow(x1,2)+pow(y1,2));
    // printf("d2: %F", d2);
    Float64 mult = d1 * d2;
    // printf(" mult: %F", mult);
    Float64 div = dotty/mult;
    // printf(" div: %F", div);
    return div;
}

// make a global function that performs SVD on a given set of raw data points and saves it in another xml file
// parameters: string name for xml file where the face data is stored, string name for where you want it to be
// saved.

void testApp::svdToXml (string rawData, string svdInfo){
    ofxXmlSettings faceData;
    if(!faceData.loadFile(rawData) ){
        ofLog(OF_LOG_NOTICE,  "unable to load mySettings.xml check data/ folder");
        return;
    }
    else {
        ofLog(OF_LOG_NOTICE, "mySettings.xml loaded!");
    }
    
    cout << (double) faceData.getValue("PT:brow", -1) << endl;
    
    // do analysis and save in another xml file
    int numFaceTags = faceData.getNumTags("PT");
    Mat matrixToEnter = Mat( 4, numFaceTags , CV_64F);
   
    for (int i = 0; i < numFaceTags; i++){
        Float64 brow = faceData.getValue("PT:mouthWidth", 0.0,i);
        matrixToEnter.at<Float64>(0,i) = brow;
    }
    for (int i = 0; i < numFaceTags; i++){
        Float64 brow = faceData.getValue("PT:mouthHeight", 0.0,i);
        matrixToEnter.at<Float64>(1,i) = brow;
    }
    for (int i = 0; i < numFaceTags; i++){
        Float64 brow = faceData.getValue("PT:eyebrowHeight", 0.0,i);
        matrixToEnter.at<Float64>(2,i) = brow;
    }
    for (int i = 0; i < numFaceTags; i++){
        Float64 brow = faceData.getValue("PT:eyeOpeness", 0.0,i);
        matrixToEnter.at<Float64>(3,i) = brow;
    }


    ofLog(OF_LOG_NOTICE, "The New Matrix" );
    printMatrix(matrixToEnter);
    
    
    // feed that Array in.
    // initialize my result arrays
    Mat matrixOutW = Mat(6, 4, CV_64F);
    Mat matrixOutU = Mat(6, 4, CV_64F);
    Mat matrixOutVt = Mat(6, 4, CV_64F);
    
    SVD::compute(matrixToEnter, matrixOutW,matrixOutU, matrixOutVt );
    // now have to adjust each array so that it is in helpful configuration
    // need W for the cosin adjustment and then I need the other two to find Bob
    // starting with reconfiguring w into the right format!
    int lengthOfMat = 2;
    Mat newMatrixOutMatW = Mat(lengthOfMat, lengthOfMat, CV_64F);
    Float64 currentNum = 0.0;
    for(int i = 0; i < lengthOfMat; i++) {
        const Float64* oldRow = matrixOutW.ptr<Float64>(i);
        const Float64* newRow = newMatrixOutMatW.ptr<Float64>(i);
        currentNum = oldRow[0];
        for(int j = 0; j < lengthOfMat; j++) {
            if (i==j){
                newMatrixOutMatW.at<Float64>(i,j)= currentNum;
            }
            else{
                newMatrixOutMatW.at<Float64>(i,j)= 0;
            }
        }
    }
    
    // invert the W matrix for calculations. We need this so we can find Bob
    Mat inverseMatrixW =newMatrixOutMatW.inv();
    
    ofLog(OF_LOG_NOTICE, "OldMatrixMat W");
    printMatrix(matrixOutW);
    
    ofLog(OF_LOG_NOTICE, "NewMatrixMat W");
    printMatrix(newMatrixOutMatW);
    
    ofLog(OF_LOG_NOTICE, "Inverse W");
    printMatrix(inverseMatrixW);
    
    // reconfigure U into the right configuration
    
    Mat newMatrixOutMatU = Mat( matrixOutU.rows, 2, CV_64F);
    Float64 currentNumU = 0.0;
    
    for(int i = 0; i < matrixOutU.rows; i++) {
        const Float64* oldRow = matrixOutU.ptr<Float64>(i);
        const Float64* newRow = newMatrixOutMatU.ptr<Float64>(i);
        for(int j = 0; j < 2 ; j++) {
            currentNumU = oldRow[j];
            newMatrixOutMatU.at<Float64>(i,j)= -1 * currentNumU;
        }
    }
    
    
    ofLog(OF_LOG_NOTICE, "OldMatrixMat u");
    printMatrix(matrixOutU);
    
    ofLog(OF_LOG_NOTICE, "NewMatrixMat u");
    printMatrix(newMatrixOutMatU);
    
    // configure V system
    // for the first two rows make sets of couple columns
    Mat newMatrixOutMatV = Mat( matrixOutVt.cols, 2, CV_64F);
    Float64 currentNumV= 0.0;
    int newI = 0;
    int newJ = 0;
    
    for(int i = 0; i < 2; i++) {
        const Float64* oldRowz = matrixOutVt.ptr<Float64>(i);
        for(int j = 0; j < matrixOutVt.cols ; j++) {
            Float64 currentNumV = oldRowz[j];
            newMatrixOutMatV.at<Float64>(j,i)= currentNumV * -1;
        }
    }
    
    ofLog(OF_LOG_NOTICE, "NewMatrixMat vt");
    printMatrix(newMatrixOutMatV);
    
    ofLog(OF_LOG_NOTICE, "OldMatrixMat vt");
    printMatrix(matrixOutVt);
    ofxXmlSettings svdData;
    // now that everything is converted into the right format, store it into another xml file
    ofLog(OF_LOG_NOTICE, "what I am sending");
    printMatrix(newMatrixOutMatV);
    saveArray(newMatrixOutMatV,svdData, "MatV" );
    saveArray(newMatrixOutMatU,svdData, "MatU" );
    saveArray(inverseMatrixW,svdData, "insverseW" );
    svdData.saveFile(svdInfo);
 
}

cv::Mat testApp::readMatrix (ofxXmlSettings xml, string filename, string matName ){
    ofLog(OF_LOG_NOTICE,"ping");
    if (xml.loadFile(filename)){
        ofLog(OF_LOG_NOTICE,"ping2");
        xml.pushTag(matName);
        int numRows  = xml.getNumTags("row");
        ofLog(OF_LOG_NOTICE,"row count");
        ofLog(OF_LOG_NOTICE, ofToString(numRows));
        xml.pushTag("row");
        int numCols  = xml.getNumTags("num");
        xml.popTag();
        ofLog(OF_LOG_NOTICE,"col count");
        ofLog(OF_LOG_NOTICE, ofToString(numCols));
        Mat returnMat = Mat( numRows, numCols, CV_64F);
        
        for (int i=0; i <numRows; i++){
            xml.pushTag("row", i);
            for (int j=0; j<numCols; j++){
                returnMat.at<Float64>(i,j)=  xml.getValue("num",0.0,j);
            }
            xml.popTag();
        }
        ofLog(OF_LOG_NOTICE,"the Matrix");
        printMatrix(returnMat);
        return returnMat;
    }
}

// make function to "find bob" (a new data set's position within an already created SVD)
// to learn what that is read http://www.igvita.com/2007/01/15/svd-recommendation-system-in-ruby/
// parameters: the matrix of new data, the name of the place where the current svd data is stored 

ofVec2f testApp::findBob(string svdData){
    ofxXmlSettings matrixData;
    Mat matV = Mat();
    matV = readMatrix(matrixData,svdData, "MatV");
    //ofLog(OF_LOG_NOTICE,"the MatV");
    //printMatrix(matV);
    Mat matU = Mat();
    matU = readMatrix(matrixData, svdData, "MatU");
    //ofLog(OF_LOG_NOTICE,"the MatU");
    //printMatrix(matU);
    Mat insverseW = Mat();
    insverseW = readMatrix(matrixData, svdData , "insverseW");
    //ofLog(OF_LOG_NOTICE,"the insverseW");
    //printMatrix(insverseW);
    // now find "bob"
    
    float mouthWidth = tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH);
    mouthWidth = ofMap(mouthWidth, 12.2738, 20.7631, 0, 17);
    float mouthHeight = tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT);
    mouthHeight = ofMap(mouthHeight, 1.00449, 8.17111, 0, 17);
    float leftEyebrowHeight = tracker.getGesture(ofxFaceTracker::LEFT_EYEBROW_HEIGHT);
    float rightEyebrowHeight = tracker.getGesture(ofxFaceTracker::RIGHT_EYEBROW_HEIGHT);
    float brow = leftEyebrowHeight + rightEyebrowHeight;
    brow = ofMap(brow, 17 , 20, 0, 13);
    float leftEyeOpeness = tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS);
    float rightEyeOpeness = tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS);
    float eyeOpeness = leftEyeOpeness + rightEyeOpeness;
    eyeOpeness = ofMap(eyeOpeness, 3.9, 6.2, 0, 8);
    //float jawOpeness = tracker.getGesture(ofxFaceTracker::JAW_OPENNESS);
    //jawOpeness = ofMap(jawOpeness, 19, 27, 0, 10);
    
    ofLog(OF_LOG_NOTICE,"past finding all the data");
    Float64 bobData[1][4] =
    {   {mouthWidth,mouthHeight,brow, eyeOpeness}
    };
    Mat bobDataMat = Mat(1, 4, CV_64F, bobData);
    
    Mat bobOutput = Mat( 1, 2, CV_64F);
    printMatrix(matU);
    ofLog(OF_LOG_NOTICE,"past finding initializedArray");
    bobOutput = bobDataMat  *  matU;
    ofLog(OF_LOG_NOTICE,"bob Output");
    //printMatrix(bobOutput);
    Mat bobOutput2 = Mat( 1, 2, CV_64F);
    bobOutput2 = bobOutput * insverseW;
    ofLog(OF_LOG_NOTICE,"bob Output2");
    printMatrix(bobOutput2);
    ofVec2f returnVev;
    const Float64* row = bobOutput2.ptr<Float64>(0);
    returnVev.set(row[0],row[1]);
    return returnVev;
}

// helper function that prints lowest and highest value and also populates the matrix
void testApp::highLowMatrixMaker (ofxXmlSettings xmlName ,cv::Mat &m, string variableName, int numFace ){
    Float64 lowestMouthWidth =200;
    Float64 highestMouthWidth =0;
    for (int i = 0; i < numFace; i++){
        Float64 brow = xmlName.getValue(variableName, 0.0,i);
        m.at<Float64>(0,i) = brow;
        if ( lowestMouthWidth - brow > .00001){
            lowestMouthWidth = brow;
        }
        else if (brow - highestMouthWidth > .00001){
            highestMouthWidth = brow;
        }
    }
    ofLog(OF_LOG_NOTICE,variableName + " magicNum:");
    ofLog(OF_LOG_NOTICE, ofToString(10 /(highestMouthWidth-lowestMouthWidth)));
}