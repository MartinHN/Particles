#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetFrameRate(25);
    ofLog() << SimdInstructionSetsInUse();
    
    particles = new MultiParticleHandler(2);
    ofColor c1 =ofColor(255,127,127);
    
//    particles->setcolor(c1,0);
//    particles->setcolor(c1.getInverted(),1);


    syphonOut.setName("main");
    initFbo();
    
    oscIn.setup(12345);
    particles->start();
    
}

void ofApp::initFbo(){
    ofFbo::Settings settings;
    settings.width = 1280;
    settings.height = 1024;
    settings.internalformat = GL_RGB;
    settings.useDepth = false;
    settings.useStencil = false;
    fboOut.allocate(settings);
    
    fboOut.begin();
    ofDisableAlphaBlending();
    ofClear(0);
    ofSetColor(0,255,0,2555);
    ofDrawRectangle(0, 0, fboOut.getWidth(), fboOut.getHeight());
    fboOut.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    processOsc();
    particles->update();

}

//--------------------------------------------------------------
void ofApp::draw(){
    fboOut.begin();
    glClearColor(0.0, 0.0, 0.0, 255.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    particles->draw();
    fboOut.end();
    syphonOut.publishTexture(&fboOut.getTexture());
    
    ofPushStyle();
    ofPushView();
    ofPushMatrix();
    
    
    ofDisableAlphaBlending();
    ofSetColor(255,255,255,255);
    fboOut.draw(0, 0,ofGetWidth(),ofGetHeight());
    
    ofPopMatrix();
    ofPopView();
    ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch(key){
        case 'x':
            particles->stopForces();
            particles->resetToInit();
            particles->startForces();
            break;
        case 'g':
            particles->renderOnGPU(!particles->isRenderingOnGPU);

            break;
        case 'o':
            particles->activateForce("origin",false);

            break;
        case 's':
            particles->stopForces();
            particles->loadModel("suzanne_tri.obj",0);
            particles->startForces();
            break;
    }
}
void ofApp::processOsc(){
    while(oscIn. hasWaitingMessages()){
        ofxOscMessage msg;
        oscIn.getNextMessage( msg );
        if(msg.getAddress() == "/attractor"){
            int attractor = msg.getArgAsInt32(0);
            string attractName = ofToString(attractor);
            int particlesToApply =attractor==0?0:-1;
            if(msg.getNumArgs() == 2){
                particles->removeAttractor(attractName,particlesToApply);
            }
            else{
                ofVec3f vec = particles->normalizedToWorld(ofVec3f(msg.getArgAsFloat(1),msg.getArgAsFloat(2),0));
                    particles->setAttractor(attractName,vec,particlesToApply);
                
            }
        }
    }
}
//--------------------------------------------------------------
void ofApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    if(button ==2){
        
        ofVec3f a = particles->screenToWorld(ofVec2f(x,y));
        particles->setAttractor("mouse",a);
        //                                  ofLog() <<         a;
    }
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    particles->removeAttractor("mouse");
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
    
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
    
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
    
}
