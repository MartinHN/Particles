//
//  ForceHandler.cpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#include "ForceHandler.hpp"


#include "ParticleHandler.hpp"




void ForceHandler::threadedFunction(){
    while(isThreadRunning()){
        uint64_t micro = ofGetElapsedTimeMillis();
        doJob();
        int toWait = owner->deltaT - (ofGetElapsedTimeMillis() - micro);
        if(toWait>0){
            //                    ofLog() <<"waiting F" << toWait;
            ofSleepMillis(toWait);
        };
    }
}
void ForceHandler::preComputeDists(){
    {
            ofScopedLock lk(mutex);
    attractors = asyncAttractors;
    }
    for( auto & a:attractors){
        Array<MatReal,1,MyMatrixType::ColsAtCompileTime> pos;
        pos(0) = a.second.x;
        pos(1) = a.second.y;
#if COLNUM >2
        pos(2) = a.second.z;
#endif
        attrVec[a.first] = owner->position.array().rowwise() - pos;
        attrNorm[a.first] = attrVec[a.first].matrix().rowwise().stableNorm();
    }
}

void ForceHandler::doJob(){
    ofScopedLock(owner->mutex);
    
    
    owner->acceleration.setZero();
    preComputeDists();
    for(auto k :activeForces){
        
        k.second->updateForce();
        
        
    }
    
}

ForceHandler::Force::Force(ForceHandler * f,string name):forceOwner(f){
    active = false;
    forceOwner->availableForces[name] = this;
}

void ForceHandler::activateForce(string name,bool t){
    if(t){
        ForceIt f = availableForces.find(name);
        if(f!=availableForces.end()){
            activeForces[name] = f->second;
        }
    }
    else{
         ForceIt f = activeForces.find(name);
        if(f!=activeForces.end()){
            activeForces.erase(name);
        }
        else activateForce(name);
    }
}

void ForceHandler::setAttractor(const string & name,ofVec3f pos){

    asyncAttractors[name] = pos;
}
void ForceHandler::removeAttractor(const string & name){
    ofScopedLock lk(mutex);
    asyncAttractors.erase(name);
}

class Origin : public ForceHandler::Force{
public:
    Origin(ForceHandler * f):Force(f,"origin"){
    }
    
//    MyMatrixType origins;
    
    float k=0.1;
    void changeNumParticles(int num) override{
        
    }
    

    void updateForce()override{
        FORCE_OWNER->acceleration += k*( FORCE_OWNER->positionInit-FORCE_OWNER->position );
        
    }
};

class Spring:public ForceHandler::Force{
public:
    Spring(ForceHandler * f):ForceHandler::Force(f,"spring"){
    }
    //    Matrix<MatReal,1,3> pos;
    double k = .1;
    
    void updateForce()override{
        double l0 = .25 * FORCE_OWNER->getWidthSpace();
        for(auto & f:forceOwner->attractors){
//            if(forceOwner->attractors[f.first].w){
            
                forceOwner->buf1D  =   forceOwner->attrNorm[f.first].eval();
                forceOwner->buf1D-=l0;
                forceOwner->buf1D*=-k;
                
            forceOwner->buf3D =  forceOwner->attrVec[f.first].eval();
            forceOwner->buf3D= forceOwner->buf3D.colwise()/(forceOwner->buf3D.matrix().rowwise().stableNorm().array());
                FORCE_OWNER->acceleration+=  forceOwner->buf3D.colwise() * forceOwner->buf1D;
//            }
        }
    }
};
class Rotate:public ForceHandler::Force{
public:
    Rotate(ForceHandler * f):ForceHandler::Force(f,"rotate"){
    }
    
    
    double k = .001;
    MyMatrixType  tmp ;
    
    void changeNumParticles(int num) override{
#ifndef FIXEDSIZE
        tmp.resize(num,MyMatrixType::ColsAtCompileTime);
#endif
    }
    
    void updateForce()override{
        for(auto & f:forceOwner->attractors){
//            if(forceOwner->attractors[f.first].w){
            
//                forceOwner->buf3D  =   forceOwner->attrVec[f.first];
//                forceOwner->buf3D.matrix().normalize();
//                for(int i = 0 ; i < FORCE_OWNER->numParticles ; i++){
//                    tmp.coeffRef(i,0) = forceOwner->attrVec[f.first].coeffRef(i,1);
//                    tmp.coeffRef(i,1) = forceOwner->attrVec[f.first].coeffRef(i,0);
//                    tmp.coeffRef(i,2) = -0.1*forceOwner->attrVec[f.first].coeffRef(i,0)*forceOwner->attrVec[f.first].coeffRef(i,1);
//                    forceOwner->buf3D.row(i) = tmp.row(i).matrix().cross(forceOwner->buf3D.row(i).matrix());
//                    
//                }
//                
//                FORCE_OWNER->acceleration += forceOwner->buf3D;
                //
               
                        MatReal *      data=   FORCE_OWNER->acceleration.data();
                        MatReal *      cpos=   FORCE_OWNER->position.data();
               
               
                        int idx = 0;
                        float r = 10,theta = .1,ctheta2 = cos(theta)*cos(theta),stheta2 = sin(theta)*sin(theta);
                        ofVec3f d,dp ;
               
                        for(int i = 0 ; i < FORCE_OWNER->numParticles ; i++){
                            int idx = i*COLNUM;
                            d.set( cpos[idx] -f.second.x,cpos[idx+1] -f.second.y,cpos[idx+2] -f.second.z);
                            d.normalize();
                            dp.set(d[1],d[0],-d[0]*d[1]);
                            dp.normalize();
                            d.cross(dp);
                            //            d.normalize();
                            data[idx]+= d[0];
                            data[idx+1] += d[1];
                        #if COLNUM >2
                            data[idx+2] += d[2];
#endif
                        }
//            }
        }
    }
    
};


class Neighbors:public ForceHandler::Force{
public:
    Neighbors(ForceHandler * f):ForceHandler::Force(f,"neighbors"){
    }
    float k=0.02,r = 0.1,l0 = 0.1;

    void changeNumParticles(int num) override{


    }
    
    void updateForce()override{
        int idx = 0;
        for(auto &l:FORCE_OWNER->activeLias){
            for(auto & ll:l){
                Array<MatReal,COLNUM,1> dist = FORCE_OWNER->position.row(ll.first) -FORCE_OWNER->position.row(idx);
                MatReal norm = dist.matrix().stableNorm();
                if(norm>0.01*FORCE_OWNER->getWidthSpace()){
                    double coef = (norm-sqrt(ll.second));
                    int sign = coef>0?1:-1;
                    coef = std::min((float)abs(coef),(float)(r*FORCE_OWNER->getWidthSpace()));
                FORCE_OWNER->acceleration.row(idx)+=k*sign*coef*(dist/norm);
                }
            }
            idx++;
        }
            
//        FORCE_OWNER->acceleration += k*( FORCE_OWNER->positionInit-FORCE_OWNER->position.bottomLeftCorner(<#Index cRows#>, <#Index cCols#>)
        
    }
    
};


void ForceHandler::initForces(){
    new Origin(this);
//        activateForce("origin");
    new Spring(this);
    activateForce("spring");
//    new Rotate(this);
//    activateForce("rotate");
    new Neighbors(this);
    activateForce("neighbors");
    
    
    
    changeNumParticles(owner->numParticles);
}



void ForceHandler::drawAttractors(){
        ofSetColor(255,0,0,255);
    for(auto a:asyncAttractors){
        ofNoFill();
        ofDrawCircle(a.second, owner->getWidthSpace()/20);
        ofFill();
        ofDrawBitmapString( a.first,a.second);
    }
}