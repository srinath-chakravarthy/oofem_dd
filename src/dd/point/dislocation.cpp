
#include "point.h"
#include "point/dislocation.h"
#include "vector.h"
#include "slipsystem.h"
#include "slipplane.h"
#include "domain.h"

namespace dd {

    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, int burgersSign,
                     typename list<Point *>::iterator nextObstacle,
                     typename list<Point *>::reverse_iterator prevObstacle) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(nextObstacle, prevObstacle),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane);
    }
    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane,
                         typename list<Point *>::iterator antecedentIt, double slipPlanePosition, int burgersSign,
                         typename list<Point *>::iterator nextObstacle,
                         typename list<Point *>::reverse_iterator prevObstacle) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(nextObstacle, prevObstacle),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane, antecedentIt);
    }

    void DislocationPoint::move() {

    }

    void DislocationPoint::spawn() {

    }

    void DislocationPoint::remove() {

    }
}
