#ifndef SLIPSYSTEM_H
#define SLIPSYSTEM_H

#include "../../../sm/EngineeringModels/DD/ddobject.h"
#include "../../../sm/EngineeringModels/DD/vector.h"
#include <cmath>
#include "../../../sm/EngineeringModels/DD/hashedregistrable.h"

namespace dd {

    class SlipPlane;

    class SlipSystem : public HashedRegistrable<SlipPlane> {
#define SLIPSYSTEM_NAME "SlipSystem"
    private:
        std::list<SlipPlane *> sPlanes;
        double angle;
        double sin;
        double cos;
        Vector2d directionVector;
        double bMag;
    public:
        /**
         * Default constructor
         */
        SlipSystem(const double & angle,
                   const double & bMag) :
            angle(angle),
            sin(::sin(angle)),
            cos(::cos(angle)),
            directionVector({cos, sin}),
            bMag(bMag) { }

        double getAngle() const { return angle; }
        double getSin() const { return sin; }
        double getCos() const { return cos; }
        double getBurgersMagnitude() const { return bMag; }
        Vector2d getDirection() const { return directionVector; }
        virtual Vector2d getPointPosition(const double & slipPlaneLocation,
                                          const Vector2d & slipPlaneOrigin) const;

        virtual string typeName() const { return SLIPSYSTEM_NAME; }
        static string staticTypeName() { return SLIPSYSTEM_NAME; }
    };
}

#endif // SLIPSYSTEM_H
