
#include "point.h"
#include "point/dislocation.h"
#include "vector.h"
#include "slipsystem.h"
#include "slipplane.h"
#include "../forcefunctor/fromdislocations.h"
#include "../forcefunctor/fromfem.h"
#include "domain.h"

namespace dd {

    void DislocationPoint::setCaches() {
        caches.clear();
        caches.reserve(2);
        caches.push_back(ForceCache(new FromDislocations(this)));
        caches.push_back(ForceCache(new FromFem(this)));
    }

    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, int burgersSign,
                     typename list<Point *>::iterator nextObstacle,
                     typename list<Point *>::reverse_iterator prevObstacle) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(nextObstacle, prevObstacle),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane);
        setCaches();
    }
    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane,
                         typename list<Point *>::iterator antecedentIt, double slipPlanePosition, int burgersSign,
                         typename list<Point *>::iterator nextObstacle,
                         typename list<Point *>::reverse_iterator prevObstacle) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(nextObstacle, prevObstacle),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane, antecedentIt);
        setCaches();
    }

    void DislocationPoint::spawn() {

    }
}
