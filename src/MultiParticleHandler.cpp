//
//  MultiParticleHandler.cpp
//  Particles
//
//  Created by Martin Hermant on 14/01/2016.
//
//

#include "MultiParticleHandler.hpp"


void MultiParticleHandler::initGL(){
    ofSetLogLevel("ofShader", ofLogLevel::OF_LOG_VERBOSE);
    
    ofDisableArbTex();
    //        ofSetVerticalSync(false);
    ofLoadImage(pointTexture, "shaders/dot.png");
    ofEnableArbTex();
    renderOnGPU(false);
    
    widthSpace = 100;
    distortFactor = .1;
    //    ofEnablePointSprites();
    cam.setDistance((widthSpace/2)*(1+1.0/distortFactor));
    cam.setFov(ofRadToDeg(2*atan(distortFactor)));
    cam.setNearClip(0.001);
    cam.setFarClip((widthSpace/2) *  2000);
    
    cam.orbit(0, 0, cam.getDistance());
    
}

void MultiParticleHandler::renderOnGPU(bool t){
    isRenderingOnGPU = t;
    if(t){
        shader.load("shaders/shaderdot");
    }
    else{
        shader.load("shaders/shadertex");
    }
    
    
}

ofVec3f MultiParticleHandler::normalizedToWorld(ofVec3f v){
    // transform v.z from world to camera coordinates
    float zFar = cam.getFarClip();
    float zNear = cam.getNearClip();
    
    double C = -(zFar+zNear)/(zFar-zNear);
    double D = -2.0*zFar*zNear/(zFar-zNear);
    float z = cam.getDistance() - v.z;
    
    //avoid nans
    if(z!=0)v.z = -C-D/-z;
    else v.z = -99999;
    
    
    v.x= v.x*2 - 1;
    v.y= v.y*2 - 1;
    //get inverse camera matrix
    ofMatrix4x4 inverseCamera;
    inverseCamera.makeInvertOf(cam.getModelViewProjectionMatrix());
    
    //convert camera to world
    return v * inverseCamera;
}



ofVec3f MultiParticleHandler::screenToWorld(ofVec3f v){
    
    // transform v.z from world to camera coordinates
    float zFar = cam.getFarClip();
    float zNear = cam.getNearClip();
    
    double C = -(zFar+zNear)/(zFar-zNear);
    double D = -2.0*zFar*zNear/(zFar-zNear);
    float z = cam.getDistance() - v.z;
    
    //avoid nans
    if(z!=0)v.z = -C-D/-z;
    else v.z = -99999;
    
    return cam.screenToWorld(v);
    
}

void MultiParticleHandler::draw(){
    
    
    glDepthMask(GL_FALSE);
    ofEnablePointSprites();
    ofEnableSmoothing();
    
//        ofEnableBlendMode(OF_BLENDMODE_ADD);

    ofEnableAlphaBlending();
    glPointSize(8);
    glLineWidth(1);
    ofSetColor(255,255,255,150);
    ofDisableDepthTest();
    cam.begin();
    shader.begin();
    
    
    
//        ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    //    ofSetColor(50,50,50,10);
    if(!isRenderingOnGPU)
        pointTexture.bind();
    for(auto &f:particlesList){
        ofSetColor(f->color,100);
        f->draw();
    }
    
    if(!isRenderingOnGPU)
        pointTexture.unbind();
    
    
    
    shader.end();
    
    
    
    for(auto &f:particlesList){
    f->forceHandler->drawAttractors();
    }
    cam.end();
    
    glDepthMask(GL_TRUE);
    ofDisablePointSprites();
}
