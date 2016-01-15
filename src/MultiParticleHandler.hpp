//
//  MultiParticleHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 14/01/2016.
//
//

#ifndef MultiParticleHandler_hpp
#define MultiParticleHandler_hpp

#include "ParticleHandler.hpp"

#define APPLY_BEGIN \
{ int i =0; \
for(auto &f:particlesList){ \
    if(idx==-1 || i==idx){

#define APPLY_END \
} i++; \
}; \
}


#define APPLY_ON_EXISTING(X) X(int idx = -1) APPLY_BEGIN \
f->X(); \
APPLY_END

#define APPLY_ON_EXISTING1(X,T) X(T Y,int idx = -1) APPLY_BEGIN \
f->X(Y); \
APPLY_END


#define APPLY_ON_EXISTING_FORCE(X) X(int idx = -1) \
APPLY_BEGIN \
f->forceHandler->X(); \
APPLY_END


#define APPLY_ON_EXISTING_FORCE1(X,T) X( T Y,int idx = -1) APPLY_BEGIN \
f->forceHandler->X(Y); \
APPLY_END

#define APPLY_ON_EXISTING_FORCE2(X,T,T2) X( T Y,T2 Z,int idx = -1) APPLY_BEGIN \
f->forceHandler->X(Y,Z); \
APPLY_END

#define EXPOSE_ARG(X,T) set##X(T Y,int idx = -1) APPLY_BEGIN \
f->X = Y; \
APPLY_END

class MultiParticleHandler{
public:
    MultiParticleHandler(int numThread){
        particlesList.resize(numThread);
        initGL();
        for(auto &f:particlesList){
            f = new ParticleHandler(this);
        }
        

    }
    ~MultiParticleHandler(){

        for(auto &f:particlesList){
            delete f ;
        }
    }
    
    void initGL();
    void renderOnGPU(bool t);
    bool isRenderingOnGPU;
        ofEasyCam cam;
    float widthSpace ;
    float distortFactor ;
    ofShader shader;
    ofTexture pointTexture;
    ofVec3f screenToWorld(ofVec3f v);
    ofVec3f normalizedToWorld(ofVec3f v);

        void draw();
    vector<ParticleHandler*> particlesList;
    
    void APPLY_ON_EXISTING(start);
    void APPLY_ON_EXISTING(update);
//    void APPLY_ON_EXISTING(draw);

    void APPLY_ON_EXISTING(stopForces);
    void APPLY_ON_EXISTING(startForces);
    void APPLY_ON_EXISTING(resetToInit);
    void APPLY_ON_EXISTING1(loadModel,string);
    
    
    void APPLY_ON_EXISTING_FORCE1(activateForce,string);
    void APPLY_ON_EXISTING_FORCE2(activateForce,string,bool);
    void APPLY_ON_EXISTING_FORCE2(setAttractor,string,ofVec3f);
    void APPLY_ON_EXISTING_FORCE1(removeAttractor,string);

    void EXPOSE_ARG(color,ofColor);
};

#endif /* MultiParticleHandler_hpp */
