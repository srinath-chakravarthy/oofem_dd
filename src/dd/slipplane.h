#pragma once

#include "hashedregistrable.h"
#include "registration.h"
#include "domain.h"
#include "vector.h"
#include <set>

namespace dd {

    class ObstaclePoint;

    class SlipPlane : public HashedRegistrable<Point> {
#define SLIPPLANE_NAME "SlipPlane"
    private:
        Domain * domain;
        Registration<SlipPlane, std::list<SlipPlane *>, SlipSystem> slipSystemRegistration;
        double __normalDistance;

    public:
        SlipPlane(Domain * domain, SlipSystem * slipSystem, double normalDistance);

        double getAngle() const;
        double getSin() const;
        double getCos() const;
        double getBurgersMagnitude() const;
        double normalDistance() const;
        Vector<2> getBurgersVector() const;
        SlipSystem * getSlipSystem() const;
        Vector<2> getOrigin() const;
        
        void moveDislocations(double dt, double b);
	
	/// Global x, y position of a point
        Vector<2> getPointPosition(const double & slipPlaneLocation) const;

        virtual string typeName() const { return SLIPPLANE_NAME; }
        static string staticTypeName() { return SLIPPLANE_NAME; }
    };
}
