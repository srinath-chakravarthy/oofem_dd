#ifndef DISLOCATION_H
#define DISLOCATION_H

#include "../dderror.h"

namespace dd {

    class Point;
    class Domain;
    class SlipPlane;
    class ObstaclePoint;

    class DislocationPoint : public Point {
#define DISLOCATIONPOINT_NAME "DislocationPoint"
    private:
        int burgersSign;
        ObstaclePoint * __positivePin = nullptr;
        ObstaclePoint * __negativePin = nullptr;
        void setCaches();
        
        void __pinTo(ObstaclePoint * obstacle, ObstaclePoint * & pinPointer);
        void __unpin(ObstaclePoint * & pinPointer);
    public:

        DislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, int burgersSign);
        DislocationPoint(Domain * domain, SlipPlane * sPlane,
                         list<Point *>::iterator antecedentIt, double slipPlanePosition, int burgersSign);
                         
        virtual int getBurgersSign() const { return burgersSign; }

		// TODO: Clear duplication

        ObstaclePoint * positivePin() const { return __positivePin; }
        ObstaclePoint * negativePin() const { return __negativePin; }
        void positivePinTo(ObstaclePoint * obstacle) { __pinTo(obstacle, __positivePin); }
        void negativePinTo(ObstaclePoint * obstacle) { __pinTo(obstacle, __negativePin); }
        void positiveUnpin() { __unpin(__positivePin); }
		void negativeUnpin() { __unpin(__negativePin); }

        virtual string typeName() const { return DISLOCATIONPOINT_NAME; }
        static string staticTypeName() { return DISLOCATIONPOINT_NAME; }
    };

}

#endif // DISLOCATION_H
