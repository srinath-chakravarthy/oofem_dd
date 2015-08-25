#ifndef OBSTACLE_H
#define OBSTACLE_H

namespace dd {

    class Point;
    class Domain;
    class SlipPlane;

    class ObstaclePoint : public Point {
#define OBSTACLEPOINT_NAME "OBSTACLEPOINT"
    public:
        ObstaclePoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition) :
            Point(slipPlanePosition) {
            setRegistrations(domain, sPlane);
        }
        ObstaclePoint(Domain * domain, SlipPlane * sPlane,
                      typename list<Point *>::iterator antecedentIt, double slipPlanePosition) :
            Point(slipPlanePosition) {
            setRegistrations(domain, sPlane, antecedentIt);
        }

        double getStrength() const { return 0; }

        virtual bool canSpawn() const { return false; }

        virtual string typeName() const { return OBSTACLEPOINT_NAME; }
        static string staticTypeName() { return OBSTACLEPOINT_NAME; }
    };

}

#endif // OBSTACLE_H
