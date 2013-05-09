//
//  collectionPortraite.h
//  faceSVD
//
//  Created by Caroline Record on 4/29/13.
//
//

#ifndef faceSVD_collectionPortraite_h
#define faceSVD_collectionPortraite_h


#include <ofxAppUtils.h>
#include <vector>
#include "../testApp.h"

using namespace ofxCv;
using namespace cv;

class collectionPortraite : public ofxScene {
    
public:
    
    // the parent
    testApp* app;
    
    // takes a reference of the parent for data access,
    // set the scene name through the base class initializer
    collectionPortraite() : ofxScene("collectionPortraite") {
        // we want setup to be called each time the scene is loaded
        setSingleSetup(false);
        app = (testApp*) ofxGetAppPtr();
    }
    
    // scene setup
    void setup() {
        // this loads all previous data
        app->XML.loadFile("mySettings.xml");
        app->svdToXml("mySettings.xml", "svdData.xml");
        // now pick out the points from the V matix and store them in an array to be drawn as points
        app->arrayOfCollectedPoints.clear();
        Mat matV = Mat();
        ofxXmlSettings newXml;
        matV = app->readMatrix(newXml, "svdData.xml", "MatV");
        ofLog(OF_LOG_NOTICE,"the MatV");
        ofVec2f coords;
        for (int i=0; i <matV.rows; i++){
            const Float64* row = matV.ptr<Float64>(i);
            coords.set(row[0],row[1]);
            app->arrayOfCollectedPoints.push_back(coords);
        }
    }
    

    
    // normal update
    void update() {

    }
      
    // draw
    void draw() {
                ofSetColor(255,255,255);
        app->cam.draw(0, 0);
        app->tracker.draw();
        //app->tracker.draw();
        ofFill();
        ofEnableAlphaBlending();
        ofSetColor(20,20,20,150);
        //x,y,width, height
        ofRect((app->cam.width/4)*3-10,20,(app->cam.width/4)-20,app->cam.height/2);
        ofSetHexColor(0x000000);
        ofDrawBitmapString("\n Distribution:", (app->cam.width/4)*3-5,20);
        // draw the points in a scaled fashion
        /*
        ofSetColor(255,255,0);
        ofFill();
        ofPolyline faceOutline = app->tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
        ofRectangle rect = faceOutline.getBoundingBox();
        faceOutline.draw();
        //vector<ofPoint> allPoints = faceOutline.getVertices();
        //ofLog(OF_LOG_NOTICE,"mouth width:");
        
        ofRect(rect); 
        */
        ofPushMatrix();
        ofTranslate((app->cam.width/4)*2-50+(app->cam.width/2),app->cam.height/4);
        
        //use the coords from the v matrix to draw a series of points
        ofFill();
        ofSetColor(255,0,0);
        //x,y,width, height
        //ofRect((app->cam.width/4)*3,20,(app->cam.width/4)-20,app->cam.height/2 );
        
        ofSetColor(192,192,192,150);
        
        ofVec2f currentCoord;
        for (int i=0; i < app->arrayOfCollectedPoints.size(); i++){
            currentCoord = app->arrayOfCollectedPoints.at(i);
            // now draw a small circle at that point.
            currentCoord *= 250;
            /*
            ofLog(OF_LOG_NOTICE,"x :" );
            ofLog(OF_LOG_NOTICE, ofToString(currentCoord.x) );
            ofLog(OF_LOG_NOTICE,"y :");
            ofLog(OF_LOG_NOTICE, ofToString(currentCoord.y) );
             */
            ofCircle(currentCoord.x,currentCoord.y,2);
        }
        ofDisableAlphaBlending();
        

        
        float mouthWidth = app->tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH);
        ofLog(OF_LOG_NOTICE,"mouth width:");
        ofLog(OF_LOG_NOTICE,ofToString(mouthWidth));
        app->currentLocation = app->findBob("svdData.xml");
        app->currentLocation *= 200;
        ofSetColor(225,0,0);
        ofCircle(app->currentLocation.x,app->currentLocation.y,2);
        // represent current position in matrix as a different color
        
        ofPopMatrix();
    }
    //clean up

    void keyPressed(int key){
        if(key == 't') {
            // take a picture
            string filename;
            string count = ofGetTimestampString();
            filename = "pictures/screen"+ count + ".png";
            // save image witOut the crap ontop
            ofImage image;
            // make the mask
            ofPolyline faceOutline = app->tracker.getImageFeature(ofxFaceTracker::FACE_OUTLINE);
            ofRectangle rect = faceOutline.getBoundingBox();
            ofPoint coord  = rect.getTopLeft();
            int x = (int)coord.x;
            int y = (int)coord.y;
            int width = (int) rect.getWidth();
            int height = (int) rect.getHeight();
            image.setFromPixels(app->cam.getPixelsRef());
            // height/width = .5
            int  newHeight = height*300/width;
            image.crop(x,y,width,height);
            image.resize(300,newHeight);
            
            
            image.saveImage(filename);
        
            //load things into an xml file
            // extract out all the variables of interest
            // the magic numbers are an attempt to scale each variable so it has equal weight
            // It was a problem before because mouth height and mouth width had too much sway
            int tagNum = app->XML.addTag("PT");
            float mouthWidth = app->tracker.getGesture(ofxFaceTracker::MOUTH_WIDTH);
            ofLog(OF_LOG_NOTICE, ofToString(mouthWidth));
            app->XML.setValue("PT:mouthWidth", ofMap(mouthWidth, 12.2738, 20.7631, 0, 17), tagNum);
            float mouthHeight = app->tracker.getGesture(ofxFaceTracker::MOUTH_HEIGHT);
            app->XML.setValue("PT:mouthHeight", ofMap(mouthHeight, 1.00449, 8.17111, 0, 17), tagNum);
            float leftEyebrowHeight = app->tracker.getGesture(ofxFaceTracker::LEFT_EYEBROW_HEIGHT);
            float rightEyebrowHeight = app->tracker.getGesture(ofxFaceTracker::RIGHT_EYEBROW_HEIGHT);
            float brow = leftEyebrowHeight + rightEyebrowHeight;
            app->XML.setValue("PT:eyebrowHeight",ofMap(brow, 17 , 20, 0, 12), tagNum);
            float leftEyeOpeness = app->tracker.getGesture(ofxFaceTracker::LEFT_EYE_OPENNESS);
            float rightEyeOpeness = app->tracker.getGesture(ofxFaceTracker::RIGHT_EYE_OPENNESS);
            float eyeOpeness = leftEyeOpeness + rightEyeOpeness;
            app->XML.setValue("PT:eyeOpeness",ofMap(eyeOpeness, 3.9, 6.2, 0, 8), tagNum);
            float jawOpeness = app->tracker.getGesture(ofxFaceTracker::JAW_OPENNESS);
            //app->XML.setValue("PT:jawOpeness",ofMap(jawOpeness, 19, 27, 0, 10), tagNum);
            app->XML.setValue("PT:filepath", filename, tagNum);

            app->XML.saveFile("mySettings.xml");
            ofLog(OF_LOG_NOTICE,"haveSaved");
            // now perform SVD on data and store in XML file
            app->svdToXml("mySettings.xml", "svdData.xml");
            // now pick out the points from the V matix and store them in an array to be drawn as points
            app->arrayOfCollectedPoints.clear();
            Mat matV = Mat();
            ofxXmlSettings newXml;
            matV = app->readMatrix(newXml, "svdData.xml", "MatV");
            ofLog(OF_LOG_NOTICE,"the MatV");
            ofVec2f coords;
            for (int i=0; i <matV.rows; i++){
                const Float64* row = matV.ptr<Float64>(i);
                coords.set(row[0],row[1]);
                app->arrayOfCollectedPoints.push_back(coords);
            }
            // we can now use these stored points to visualize what our face distribution is.
        }
    }
    
};
#endif
