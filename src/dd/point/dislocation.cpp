
#include "point.h"
#include "point/dislocation.h"
#include "vector.h"
#include "slipsystem.h"
#include "slipplane.h"
#include "../forcefunctor/fromdislocations.h"
#include "../forcefunctor/fromfem.h"
#include "domain.h"
#include "point/obstacle.h"

namespace dd {

    void DislocationPoint::setCaches() {
        caches.clear();
        caches.reserve(2);
        caches.push_back(ForceCache(FromDislocations::getInstance()));
        caches.push_back(ForceCache(FromFem::getInstance()));
    }

    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, int burgersSign) :
            Point(slipPlanePosition),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane);
        setCaches();
    }
    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane,
                         typename list<Point *>::iterator antecedentIt, double slipPlanePosition, int burgersSign) :
            Point(slipPlanePosition),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane, antecedentIt);
        setCaches();
    }

    void DislocationPoint::pinTo(ObstaclePoint * obstacle) {
        if(__pin) {
            DdError::exception("Already pinned.");
        }
        if(__pin == obstacle) { return; }
        __pin = obstacle;
        obstacle->pin(this);
    }
}
