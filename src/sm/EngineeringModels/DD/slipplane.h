#pragma once

#include "../../../sm/EngineeringModels/DD/hashedregistrable.h"
#include "../../../sm/EngineeringModels/DD/registration.h"
#include "../../../sm/EngineeringModels/DD/domain.h"
#include "../../../sm/EngineeringModels/DD/vector.h"

namespace dd {

    class ObstaclePoint;

    class SlipPlane : public HashedRegistrable<Point> {
#define SLIPPLANE_NAME "SlipPlane"
    private:
        Domain * domain;
        Registration<SlipPlane, SlipSystem> slipSystemRegistration;
        Vector2d origin;

    public:
        SlipPlane(Domain * domain, SlipSystem * slipSystem, Vector2d origin);

        double getAngle() const;
        double getSin() const;
        double getCos() const;
        double getBurgersMagnitude() const;
        Vector2d getBurgersVector() const;
        SlipSystem * getSlipSystem() const;
        Vector2d getOrigin() const;
	
	/// Global x, y position of a point
        Vector2d getPointPosition(const double & slipPlaneLocation) const;

        virtual string typeName() const { return SLIPPLANE_NAME; }
        static string staticTypeName() { return SLIPPLANE_NAME; }
    };
}
