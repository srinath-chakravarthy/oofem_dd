#ifndef SOURCE_H
#define SOURCE_H

#include "point.h"

namespace dd {
    class Domain;
    class SlipPlane;

    class SourcePoint : public Point {
#define SOURCEPOINT_NAME "SourcePoint"
    private:
        double __strength;
        double __length;
        double __timer;
        void setCaches();
    public:

        SourcePoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, double strength = 0, double length = 0,
        			double timer = 0) :
            Point(slipPlanePosition),
            __strength(strength),
            __length(length),
            __timer(timer) {
            setRegistrations(domain, sPlane);
            setCaches();
        }
        SourcePoint(Domain * domain, SlipPlane * sPlane,
                    list<Point *>::iterator antecedentIt, double slipPlanePosition, double strength = 0, double length = 0,
                    double timer = 0) :
            Point(slipPlanePosition),
            __strength(strength),
            __length(length),
            __timer(timer) {
            setRegistrations(domain, sPlane, antecedentIt);
            setCaches();
        }
        
        double strength() const { return __strength; }
        double length() const { return __length; }
        double timer() const { return __timer; }
        
        void spawn(const double & dt, const double & tnuc);

        virtual string typeName() const { return SOURCEPOINT_NAME; }
        static string staticTypeName() { return SOURCEPOINT_NAME; }
    };
}

#endif // SOURCE_H
