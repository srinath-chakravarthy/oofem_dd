#ifndef SLIPSYSTEM_H
#define SLIPSYSTEM_H

#include "ddobject.h"
#include "vector.h"
#include <cmath>
#include "registrable/singletonregistrable.h"
#include <set>

namespace dd {

    class SlipPlane;

    class SlipSystem : public SingletonRegistrable<SlipPlane> {
#define SLIPSYSTEM_NAME "SlipSystem"
    private:
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
        std::list<SlipPlane *> & getSlipPlanes() { return container(); }
        Vector<2> getDirection() const { return directionVector; }
        Vector<2> normalDirection() const { return {directionVector[0], -directionVector[1]}; }
        virtual Vector<2> getPointPosition(const double & slipPlaneLocation,
                                           const Vector2d & slipPlaneOrigin) const;
        
        virtual string typeName() const { return SLIPSYSTEM_NAME; }
        static string staticTypeName() { return SLIPSYSTEM_NAME; }
    };
}

#endif // SLIPSYSTEM_H
