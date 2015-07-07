#ifndef VIRTUALDISLOCATION_H
#define VIRTUALDISLOCATION_H

#include "../../../../sm/EngineeringModels/DD/point.h"
#include "../../../../sm/EngineeringModels/DD/point/source.h"
#include "../../../../sm/EngineeringModels/DD/betweenpoints.h"

namespace dd {

    class Domain;
    class SlipPlane;

    class VirtualDislocationPoint : public Point, public BetweenPoints<Point> {
#define VIRTUALDISLOCATION_NAME "VirtualDislocation"
    public:

        VirtualDislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition,
                                list<Point *>::iterator next,
                                list<Point *>::reverse_iterator prev) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(next, prev) {
            setRegistrations(domain, sPlane);
        }
        VirtualDislocationPoint(Domain * domain, SlipPlane * sPlane,
                                list<Point *>::iterator antecedentIt, double slipPlanePosition,
                                list<Point *>::iterator next,
                                list<Point *>::reverse_iterator prev) :
            Point(slipPlanePosition),
            BetweenPoints<Point>(next, prev) {
            setRegistrations(domain, sPlane, antecedentIt);
        }

        virtual string typeName() const { return VIRTUALDISLOCATION_NAME; }
        static string staticTypeName() { return VIRTUALDISLOCATION_NAME; }
    };
}

#endif // VIRTUALDISLOCATION_H
