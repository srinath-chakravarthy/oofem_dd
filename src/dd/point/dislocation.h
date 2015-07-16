#ifndef DISLOCATION_H
#define DISLOCATION_H

#include "betweenpoints.h"

namespace dd {

    class Point;
    class Domain;
    class SlipPlane;
    class ObstaclePoint;

    class DislocationPoint : public Point, public BetweenPoints<Point> {
#define DISLOCATIONPOINT_NAME "DislocationPoint"
    private:
        int burgersSign;
        void setCaches();
    public:

        DislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, int burgersSign,
                         list<Point *>::iterator nextObstacle,
                         list<Point *>::reverse_iterator prevObstacle);
        DislocationPoint(Domain * domain, SlipPlane * sPlane,
                         list<Point *>::iterator antecedentIt, double slipPlanePosition, int burgersSign,
                         list<Point *>::iterator nextObstacle,
                         list<Point *>::reverse_iterator prevObstacle);
                         
        virtual int getBurgersSign() const { return burgersSign; }

        virtual bool canMove() const { return true; }
        virtual void move();

        virtual bool canSpawn() const { return true; }
        virtual void spawn();

        virtual bool canRemove() const { return true; }
        virtual void remove();

        virtual string typeName() const { return DISLOCATIONPOINT_NAME; }
        static string staticTypeName() { return DISLOCATIONPOINT_NAME; }
    };

}

#endif // DISLOCATION_H
