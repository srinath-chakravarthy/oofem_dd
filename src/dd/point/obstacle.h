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
        double __strength;
        bool checkRelease(const DislocationPoint * point) const;
    public:
        ObstaclePoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, double strength = 0) :
            Point(slipPlanePosition), __strength(strength) {
            setRegistrations(domain, sPlane);
        }
        ObstaclePoint(Domain * domain, SlipPlane * sPlane,
                      typename list<Point *>::iterator antecedentIt, double slipPlanePosition, double strength = 0) :
            Point(slipPlanePosition), __strength(strength) {
            setRegistrations(domain, sPlane, antecedentIt);
        }
        
        double strength() const { return __strength; }
        
        virtual DislocationPoint * negativePinned() const { return __negativePinned; }
        virtual DislocationPoint * positivePinned() const { return __positivePinned; }
        
        /**
         * Pin the given dislocation coming from the given side on the slip plane
         *
         * @param point		The dislocation to be pinned
         * @param fromSide	1 or -1, denoting the slipPlaneDirection the point is coming from
         */
        virtual void pin(DislocationPoint * point);
        virtual void release();

        virtual string typeName() const { return OBSTACLEPOINT_NAME; }
        static string staticTypeName() { return OBSTACLEPOINT_NAME; }
    };

}

#endif // OBSTACLE_H
