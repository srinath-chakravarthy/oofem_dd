#ifndef VIRTUALDISLOCATION_H
#define VIRTUALDISLOCATION_H

#include "point.h"
#include "point/source.h"

namespace dd {

    class Domain;
    class SlipPlane;

    class VirtualDislocationPoint : public Point {
#define VIRTUALDISLOCATION_NAME "VirtualDislocation"
    public:

        VirtualDislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition) :
            Point(slipPlanePosition) {
            setRegistrations(domain, sPlane);
        }
        VirtualDislocationPoint(Domain * domain, SlipPlane * sPlane,
                                list<Point *>::iterator antecedentIt, double slipPlanePosition) :
            Point(slipPlanePosition) {
            setRegistrations(domain, sPlane, antecedentIt);
        }

        virtual string typeName() const { return VIRTUALDISLOCATION_NAME; }
        static string staticTypeName() { return VIRTUALDISLOCATION_NAME; }
    };
}

#endif // VIRTUALDISLOCATION_H
