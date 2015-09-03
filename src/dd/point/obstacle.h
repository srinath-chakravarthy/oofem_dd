#ifndef OBSTACLE_H
#define OBSTACLE_H

namespace dd {

    class Point;
    class Domain;
    class SlipPlane;
    class DislocationPoint;

    class ObstaclePoint : public Point {
#define OBSTACLEPOINT_NAME "OBSTACLEPOINT"
    // INVARIANT: If exists, __negativePinned has positive BurgersSign
    // INVARIANT: If exists, __positivePinned has negative BurgersSign
    private:
        DislocationPoint * __negativePinned = nullptr;
        DislocationPoint * __positivePinned = nullptr;
        
        bool checkRelease(const DislocationPoint * point) const;
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
        
        virtual double strength() const { return 0; }
        
        virtual DislocationPoint * negativePinned() const { return __negativePinned; }
        virtual DislocationPoint * positivePinned() const { return __positivePinned; }
        virtual void pin(DislocationPoint * point);
        virtual void release();

        virtual string typeName() const { return OBSTACLEPOINT_NAME; }
        static string staticTypeName() { return OBSTACLEPOINT_NAME; }
    };

}

#endif // OBSTACLE_H
