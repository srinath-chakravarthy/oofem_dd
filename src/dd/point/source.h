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
    public:

        SourcePoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, double strength = 0) :
            Point(slipPlanePosition),
            __strength(strength) {
            setRegistrations(domain, sPlane);
        }
        SourcePoint(Domain * domain, SlipPlane * sPlane,
                    list<Point *>::iterator antecedentIt, double slipPlanePosition, double strength = 0) :
            Point(slipPlanePosition),
            __strength(strength) {
            setRegistrations(domain, sPlane, antecedentIt);
        }
        
        double strength() const { return __strength; }

        

        virtual string typeName() const { return SOURCEPOINT_NAME; }
        static string staticTypeName() { return SOURCEPOINT_NAME; }
    };
}

#endif // SOURCE_H
