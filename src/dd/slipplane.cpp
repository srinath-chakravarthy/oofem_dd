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
        std::vector<double> projections;
        projections.reserve(dislocs.size());
        
        for(auto disloc : dislocs) {
            Vector<2> force, forceGradient;
            Vector<3> stress;
            disloc->sumCaches(force, forceGradient, stress);
            double deltaPos = (1 / b) * (force.abs() / (1 - (dt * forceGradient.abs() / b)));
            projections.push_back(disloc->getSlipPlanePosition() + deltaPos);
        }
        
        auto it = --dislocs.end();
        for(int i = dislocs.size() - 1; i >= 1; i--) {
            auto nextIt = it;
            nextIt--;
            
            if((*it)->getBurgersSign() == (*nextIt)->getBurgersSign()) {
                projections[i - 1] = std::min(projections[i - 1],
                                           projections[i] - 2 * getBurgersMagnitude());
                
            }
            else {
                if(projections[i] - projections[i - 1] < 6 * b * getBurgersMagnitude()) {
                    dislocs.erase(it);
                    it = nextIt;
                    it--;
                    i--;
                    dislocs.erase(nextIt);
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
