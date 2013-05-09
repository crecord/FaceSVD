//
//  collectionManyFace.h
//  faceSVD
//
//  Created by Caroline Record on 4/29/13.
//
//

#ifndef faceSVD_collectionManyFace_h
#define faceSVD_collectionManyFace_h

#pragma once

#include <ofxAppUtils.h>

#include "../testApp.h"

class collectionManyFace : public ofxScene {
    
public:
    
    // takes a reference of the parent for data access,
    // set the scene name through the base class initializer
    collectionManyFace() : ofxScene("collectionManyFace") {
        app = (testApp*) ofxGetAppPtr();
        // we want setup to be called each time the scene is loaded
        setSingleSetup(false);
    }
    
    // scene setup
    void setup() {
        
    }
    

    
    // normal update
    void update() {
       
    }
    
    
    // draw
    void draw() {
        
    }
    
    
    // the parent
    testApp* app;
    

};


#endif
