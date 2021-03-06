//
//  ForceHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#ifndef ForceHandler_hpp
#define ForceHandler_hpp

#include <stdio.h>
#include "ofMain.h"


#include "Config.h"

class ParticleHandler;
#define FORCE_OWNER this->forceOwner->owner


class ForceHandler : public ofThread {

    public :
    
    ForceHandler(ParticleHandler * p):owner(p){
        
    }
    
    void threadedFunction() override;
    
    void initForces();
    void doJob();
    void setAttractor(const string & name,ofVec3f pos);
    void removeAttractor(const string & name);
    
    ParticleHandler * owner;

    
    map<string,ofVec3f> asyncAttractors;
    
    map<string,    Array<MatReal,ROWTYPE,1> > attrNorm;
    map<string,    Array<MatReal,ROWTYPE,COLNUM> > attrVec;
    Array<MatReal,ROWTYPE,1> buf1D;
    Array<MatReal,ROWTYPE,COLNUM> buf3D;
    
    void preComputeDists();
    void changeNumParticles(int num){
        for(auto &f:activeForces){
            f.second->changeNumParticles(num);
        }
    };
    void drawAttractors();
    class Force{
    public:

        Force(ForceHandler * f,string name);
        bool active;
        virtual void activate(){};
        
        virtual void updateForce() = 0;
        virtual void changeNumParticles(int num){};
        ForceHandler * forceOwner;
    };
    
    map<string,Force* > availableForces;
    map<string,Force* > activeForces;
    typedef map<string,Force* >::iterator ForceIt;
    void activateForce(string name,bool t=true);


    

        map<string,ofVec3f> attractors;
    
    
    

};
#endif /* ForceHandler_hpp */
