#ifndef SOURCE_H
#define SOURCE_H

#include "point.h"

namespace dd {
    class Domain;
    class SlipPlane;

    class SourcePoint : public Point {
#define SOURCEPOINT_NAME "SourcePoint"
    private:
        double strength;
        double length;
    public:

        SourcePoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, double strength, double length) :
            Point(slipPlanePosition),
            strength(strength),
            length(length) {
            setRegistrations(domain, sPlane);
        }
        SourcePoint(Domain * domain, SlipPlane * sPlane,
                    list<Point *>::iterator antecedentIt, double slipPlanePosition, double strength, double length) :
            Point(slipPlanePosition),
            strength(strength),
            length(length) {
            setRegistrations(domain, sPlane, antecedentIt);
        }

        double getStrength() const { return 0; }
        double getLength() const { return 0; }

        virtual bool canSpawn() const { return true; }
        virtual void spawn();

        virtual bool canRemove() const { return true; }

        virtual string typeName() const { return SOURCEPOINT_NAME; }
        static string staticTypeName() { return SOURCEPOINT_NAME; }
    };
}

#endif // SOURCE_H
