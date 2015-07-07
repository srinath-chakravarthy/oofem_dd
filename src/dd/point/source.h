#ifndef SOURCE_H
#define SOURCE_H

#include "point.h"
#include "betweenpoints.h"

namespace dd {
    class Domain;
    class SlipPlane;

    class SourcePoint : public Point, public BetweenPoints<Point> {
#define SOURCEPOINT_NAME "SourcePoint"
    private:
        double strength;
        double length;
    public:

        SourcePoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, double strength, double length,
                    list<Point *>::iterator next,
                    list<Point *>::reverse_iterator prev) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(next, prev),
            strength(strength),
            length(length) {
            setRegistrations(domain, sPlane);
        }
        SourcePoint(Domain * domain, SlipPlane * sPlane,
                    list<Point *>::iterator antecedentIt, double slipPlanePosition, double strength, double length,
                    list<Point *>::iterator next,
                    list<Point *>::reverse_iterator prev) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(next, prev),
            strength(strength),
            length(length) {
            setRegistrations(domain, sPlane, antecedentIt);
        }

        double getStrength() const { return 0; }
        double getLength() const { return 0; }

        virtual bool canMove() const { return false; }

        virtual bool canSpawn() const { return true; }
        virtual void spawn();

        virtual bool canRemove() const { return true; }

        virtual string typeName() const { return SOURCEPOINT_NAME; }
        static string staticTypeName() { return SOURCEPOINT_NAME; }
    };
}

#endif // SOURCE_H
