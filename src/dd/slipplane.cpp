#include "slipplane.h"
#include "slipsystem.h"
#include "point.h"
#include "point/obstacle.h"
#include "point/dislocation.h"

#include <cmath>

namespace dd {

    SlipPlane::SlipPlane(Domain * domain, SlipSystem * slipSystem, Vector2d origin) :
        domain(domain),
        slipSystemRegistration(this, slipSystem),
        origin(origin) { }

    SlipSystem * SlipPlane::getSlipSystem() const { return slipSystemRegistration.getTarget(); }
    double SlipPlane::getAngle() const { return getSlipSystem()->getAngle(); }
    double SlipPlane::getCos() const { return getSlipSystem()->getCos(); }
    double SlipPlane::getSin() const { return getSlipSystem()->getSin(); }
    double SlipPlane::getBurgersMagnitude() const { return getSlipSystem()->getBurgersMagnitude(); }
    Vector2d SlipPlane::getBurgersVector() const { return getSlipSystem()->getDirection() * getBurgersMagnitude(); }
    Vector2d SlipPlane::getOrigin() const { return origin; }
    Vector2d SlipPlane::getPointPosition(const double & slipPlaneLocation) const {
        return getSlipSystem()->getPointPosition(slipPlaneLocation, origin);
    }
    
    void SlipPlane::moveDislocations(double dt, double b) {
        std::list<Point *> & dislocs = HashedRegistrable<Point>::getContainer<DislocationPoint>();
        std::list<Point *> & obs = HashedRegistrable<Point>::getContainer<ObstaclePoint>();
        std::vector<double> projections;
        projections.reserve(dislocs.size());
        std::list<Point *>::iterator it;
       
        // Release pins
        for(auto ob : obs) {
            static_cast<ObstaclePoint *>(ob)->release();
        }
        
        // Calculate new positions
        for(auto disloc : dislocs) {
            Vector<2> force, forceGradient;
            Vector<3> stress;
            disloc->sumCaches(force, forceGradient, stress);
            double deltaPos = (1 / b) * (force.abs() / (1 - (dt * forceGradient.abs() / b)));
            projections.push_back(disloc->getSlipPlanePosition() + deltaPos);
        }
        
        // Negative pinning
        it = dislocs.begin();
        long long index = 0;
        for(auto nextOb = obs.begin(); it != dislocs.end() && nextOb != obs.end(); nextOb++) {
            ObstaclePoint * currentObstacle = static_cast<ObstaclePoint *>(*nextOb);
            if(currentObstacle->negativePinned()) { continue; }
            
            bool hasDisToLeft = false;
            while(it != dislocs.end() && (*it)->getSlipPlanePosition() < currentObstacle->getSlipPlanePosition()) {
                hasDisToLeft = true;
                it++;
                index++;
            }
            
            if(hasDisToLeft) {
                it--;
                index--;
                if(projections[index] > currentObstacle->getSlipPlanePosition() - 2 * currentObstacle->getBurgersMagnitude()) {
                    currentObstacle->pin(static_cast<DislocationPoint *>(*it));
                }
                it++;
                index++;
            }
        }
        
        // Positive pinning
        auto revIt = dislocs.rbegin();
        index = projections.size() - 1;
        for(auto prevOb = obs.rbegin(); revIt != dislocs.rend() && prevOb != obs.rend(); prevOb++) {
            ObstaclePoint * currentObstacle = static_cast<ObstaclePoint *>(*prevOb);
            if(currentObstacle->positivePinned()) { continue; }
            
            bool hasDisToRight = false;
            while(revIt != dislocs.rend() && (*revIt)->getSlipPlanePosition() < currentObstacle->getSlipPlanePosition()) {
                hasDisToRight = true;
                revIt++;
                index--;
            }
            
            if(hasDisToRight) {
                revIt--;
                index++;
                if(projections[index] < currentObstacle->getSlipPlanePosition() + 2 * currentObstacle->getBurgersMagnitude()) {
                    currentObstacle->pin(static_cast<DislocationPoint *>(*revIt));
                }
                revIt++;
                index--;
            }
            prevOb++;
        }
        
        // Sort
        it = --dislocs.end();
        for(int i = dislocs.size() - 1; i >= 1; i--) {
            auto nextIt = it;
            nextIt--;
            
            if(static_cast<DislocationPoint *>(*it)->getPin()) { continue; }
            
            if((*it)->getBurgersSign() == (*nextIt)->getBurgersSign()) {
                projections[i - 1] = std::min(projections[i - 1],
                                           projections[i] - 2 * getBurgersMagnitude());
                
            }
            else {
                if(projections[i] - projections[i - 1] < 6 * b * getBurgersMagnitude()) {
                    i--;
                    // dislocs.erase(it);
                    delete (*it);
                    it = nextIt;
                    it--;
                    // dislocs.erase(nextIt);
                    delete (*nextIt);
                    continue;
                }
            }  
            
            (*it)->updateLocation(PointLog((*it)->getSlipPlane(), projections[i]));
            it = nextIt;      
        }
        
        if(it == dislocs.begin()) {
            (*it)->updateLocation(PointLog((*it)->getSlipPlane(), projections[0]));
        }
    }

}
