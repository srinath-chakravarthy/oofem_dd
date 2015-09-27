#include "slipplane.h"
#include "slipsystem.h"
#include "point.h"
#include "point/obstacle.h"
#include "point/dislocation.h"

#include <cmath>
#include <sstream>
#include <iostream>

namespace dd {

    SlipPlane::SlipPlane(Domain * domain, SlipSystem * slipSystem, double normalDistance) :
        domain(domain),
        slipSystemRegistration(this, slipSystem),
        __normalDistance(normalDistance) { }

    SlipSystem * SlipPlane::getSlipSystem() const { return slipSystemRegistration.target(); }
    double SlipPlane::getAngle() const { return getSlipSystem()->getAngle(); }
    double SlipPlane::getCos() const { return getSlipSystem()->getCos(); }
    double SlipPlane::getSin() const { return getSlipSystem()->getSin(); }
    double SlipPlane::getBurgersMagnitude() const { return getSlipSystem()->getBurgersMagnitude(); }
    double SlipPlane::normalDistance() const { return __normalDistance; }
    Vector2d SlipPlane::getBurgersVector() const { return getSlipSystem()->getDirection() * getBurgersMagnitude(); }
    Vector2d SlipPlane::getOrigin() const { return getSlipSystem()->normalDirection() * __normalDistance; }
    Vector2d SlipPlane::getPointPosition(const double & slipPlaneLocation) const {
        return getSlipSystem()->getPointPosition(slipPlaneLocation, getOrigin());
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
            double deltaPos;
            if(static_cast<DislocationPoint *>(disloc)->pinned()) {
                deltaPos = 0;
            }
            else {
                Vector<2> force, forceGradient;
                Vector<3> stress;
                disloc->sumCaches(force, forceGradient, stress);
                deltaPos = (1 / b) * (force[0] / (1 - (dt * forceGradient[0] / b))) * dt;
            }
            projections.push_back(disloc->slipPlanePosition() + deltaPos);
        }

        //
        // TODO: Clean the pinning; extract it
        //

        // Negative pinning
//         it = dislocs.begin();
//         long long index = 0;
//         for(auto nextOb = obs.begin(); it != dislocs.end() && nextOb != obs.end(); nextOb++) {
//             ObstaclePoint * currentObstacle = static_cast<ObstaclePoint *>(*nextOb);
//             if(currentObstacle->negativePinned()) {
//                 continue;
//             }
// 
//             bool hasDisToLeft = false;
//             while(it != dislocs.end() && (*it)->slipPlanePosition() < currentObstacle->slipPlanePosition()) {
//                 hasDisToLeft = true;
//                 it++;
//                 index++;
//             }
// 
//             if(hasDisToLeft) {
//                 it--;
//                 index--;
//                 if(projections[index] > currentObstacle->slipPlanePosition() - 2 * currentObstacle->getBurgersMagnitude()) {
//                     currentObstacle->pin(static_cast<DislocationPoint *>(*it));
//                 }
//                 it++;
//                 index++;
//             }
//         }
// 
//         // Positive pinning
//         auto revIt = dislocs.rbegin();
//         index = projections.size() - 1;
//         for(auto prevOb = obs.rbegin(); revIt != dislocs.rend() && prevOb != obs.rend(); prevOb++) {
//             ObstaclePoint * currentObstacle = static_cast<ObstaclePoint *>(*prevOb);
//             if(currentObstacle->positivePinned()) {
//                 continue;
//             }
// 
//             bool hasDisToRight = false;
//             while(revIt != dislocs.rend() && (*revIt)->slipPlanePosition() < currentObstacle->slipPlanePosition()) {
//                 hasDisToRight = true;
//                 revIt++;
//                 index--;
//             }
// 
//             if(hasDisToRight) {
//                 revIt--;
//                 index++;
//                 if(projections[index] < currentObstacle->slipPlanePosition() + 2 * currentObstacle->getBurgersMagnitude()) {
//                     currentObstacle->pin(static_cast<DislocationPoint *>(*revIt));
//                 }
//                 revIt++;
//                 index--;
//             }
//             prevOb++;
//         }

        // Sort
        it = --dislocs.end();
        bool pushRight = true;
        for(int i = dislocs.size() - 1; i >= 1; i--) {
            auto nextIt = it;
            nextIt--;

            DislocationPoint * dis = static_cast<DislocationPoint *>(*it);
            DislocationPoint * nextDis = static_cast<DislocationPoint *>(*nextIt);

            if(dis->negativePin()) {
                continue;
            }
            if(dis->positivePin()) {
                pushRight = false;
            }


            if(dis->getBurgersSign() == nextDis->getBurgersSign()) {
                if(pushRight) {
                    projections[i] = std::max(projections[i],
                                            projections[i - 1] + 2 * getBurgersMagnitude());
                }
                else {
                    projections[i - 1] = std::min(projections[i - 1],
                                                projections[i] - 2 * getBurgersMagnitude());
                }
            }
            else {
                pushRight = true;
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

            (*it)->updateLocation(PointLog((*it)->slipPlane(), projections[i]));
            it = nextIt;
        }

        if(it == dislocs.begin() && it != dislocs.end()) {
            (*it)->updateLocation(PointLog((*it)->slipPlane(), projections[0]));
        }
    }

    string SlipPlane::__dumpToString(std::list<Point *> & container) {
        std::stringstream ss;
        for(Point * p : container) {
            ss << p->slipPlanePosition() << " ";
        }
        return ss.str();
    }
}
