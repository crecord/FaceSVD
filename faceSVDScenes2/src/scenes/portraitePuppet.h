//
//  portraitePuppet.h
//  faceSVD
//
//  Created by Caroline Record on 4/29/13.
//
//

#ifndef faceSVD_portraitePuppet_h
#define faceSVD_portraitePuppet_h

#pragma once

#include <ofxAppUtils.h>

#include "../testApp.h"

class portraitePuppet : public ofxScene {
    
public:
    testApp* app;
    
    // takes a reference of the parent app for data access,
    // set the scene name through the base class initializer
    portraitePuppet() : ofxScene("portraitePuppet") {
        app = (testApp*) ofxGetAppPtr();
        // we want setup to be called each time the scene is loaded
        setSingleSetup(false);
    }
    
    // scene setup
    void setup() {
		app->imgPath = "pictures/screen2013-05-01-02-14-53-468.png"; 
    }
    

    
    // normal update
    void update() {
        
    }
    
    // called when scene is exiting
    void updateExit() {
		
    }
    
    // draw
    void draw() {
       
        //app->cam.draw(0, 0);
        //missing matV
        if( app->imgCount % 10==0){
        ofVec2f newBobInTown = app->findBob("svdData.xml");
        ofxXmlSettings matrixData;
        Mat matV = Mat();
        matV = app->readMatrix(matrixData,"svdData.xml", "MatV");
        Float64 xTwo =  newBobInTown.x;
        Float64 yTwo = newBobInTown.y;
        Float64 sim = 0.0;
        Float64 highestSimValue =0.0;
        int highestSimIndex = 0;
        for(int xed=0; xed < matV.rows; xed++){
            Float64* xOne =matV.ptr<Float64>(xed,0);
            Float64* yOne = matV.ptr<Float64>(xed,1);
            sim = app->cosSim(xTwo,yTwo,*xOne,*yOne);
            if (sim > .9 ){
                if (sim - highestSimValue > .000001 ){
                    highestSimValue = sim;
                    highestSimIndex= xed;
                }
            }
            printf("Cos Sim %F",sim);
            
        }
        printf("Highest Sim Index %i",highestSimIndex);
        printf("High Sim %F",highestSimValue);
        // now get the image that most closely coorilates
        matrixData.clear();
        matrixData.loadFile("mySettings.xml");
        matrixData.pushTag("PT",highestSimIndex);
        app->imgPath = matrixData.getValue("filepath", "m");
        // now you have the matching img path draw it somewhere to the screen
        }
        app->mostSimilar.loadImage(app->imgPath);
        ofSetColor(255,255,255);
        int widthImg =app->mostSimilar.getWidth(); 
        int heightImg = app->mostSimilar.getHeight();
        app->mostSimilar.setAnchorPoint(widthImg/2, heightImg/2);
        int width = app->getRenderWidth();
        int height = app->getRenderHeight();
        app->mostSimilar.draw(width/2,height/2);
        app->tracker.draw();
        
        app->imgCount ++;
         
    }
    
    
    // the parent
    
    
   
};


#endif
