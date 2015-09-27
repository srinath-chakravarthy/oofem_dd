#pragma once

#include "registrable/hashedregistrable.h"
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
        
        /**
         * Dumps the locations of the points in the given container into a string
         * 
         * @param pointsPtr     Pointer to the container
         */
        static string __dumpToString(std::list<Point *> & container);
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
        
        /**
         * Dumps the points of the given type into a string
         * 
         * @param T     The type of points to be dumped.
         */
        template<typename T>
        string dumpToString() { return __dumpToString(getContainer<T>()); }

        virtual string typeName() const { return SLIPPLANE_NAME; }
        static string staticTypeName() { return SLIPPLANE_NAME; }
    };
}
