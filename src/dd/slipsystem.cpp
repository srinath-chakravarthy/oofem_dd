#include "slipsystem.h"
#include "slipplane.h"
#include "vector.h"
#include "point.h"

namespace dd {

    Vector2d SlipSystem::getPointPosition(const double & slipPlaneLocation,
                                          const Vector2d & slipPlaneOrigin) const {
        return directionVector * slipPlaneLocation + slipPlaneOrigin;
    }
}
