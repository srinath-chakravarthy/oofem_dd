#include "slipplane.h"
#include "slipsystem.h"
#include "point.h"
#include "point/obstacle.h"

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

}
