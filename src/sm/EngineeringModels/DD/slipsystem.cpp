#include "../../../sm/EngineeringModels/DD/slipsystem.h"
#include "../../../sm/EngineeringModels/DD/slipplane.h"
#include "../../../sm/EngineeringModels/DD/vector.h"
#include "../../../sm/EngineeringModels/DD/point.h"

namespace dd {

    Vector2d SlipSystem::getPointPosition(const double & slipPlaneLocation,
                                          const Vector2d & slipPlaneOrigin) const {
        return directionVector * slipPlaneLocation + slipPlaneOrigin;
    }
}
