//
//  ManyFacePuppet.h
//  faceSVD
//
//  Created by Caroline Record on 4/29/13.
//
//

#ifndef faceSVD_ManyFacePuppet_h
#define faceSVD_ManyFacePuppet_h

#pragma once

#include <ofxAppUtils.h>

#include "../testApp.h"

class ManyFacePuppet : public ofxScene {
    
public:
    
    // takes a reference of the parent app for data access,
    // set the scene name through the base class initializer
    ManyFacePuppet() : ofxScene("ManyFacePuppet") {
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
