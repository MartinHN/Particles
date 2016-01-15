//
//  PhysicsHandler.hpp
//  Particles
//
//  Created by Martin Hermant on 03/01/2016.
//
//

#ifndef PhysicsHandler_hpp
#define PhysicsHandler_hpp

#include <stdio.h>

#include "ofMain.h"


#include "Config.h"

class ParticleHandler;




class PhysicsHandler  : public ofThread{
    public :
    
    PhysicsHandler(ParticleHandler * p):owner(p){
        
    }
    
    void threadedFunction() override;
    
    void doJob();
    
    double actualDelay;
    
    
    float fr = .9,maxVel = 0;
    
    
    ParticleHandler * owner;
};
#endif /* PhysicsHandler_hpp */
