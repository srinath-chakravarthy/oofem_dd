
#include "point.h"
#include "point/dislocation.h"
#include "vector.h"
#include "slipsystem.h"
#include "slipplane.h"
#include "../forcefunctor/fromdislocations.h"
#include "../forcefunctor/fromfem.h"
#include "../forcefunctor/fromfemmock.h"
#include "domain.h"
#include "point/obstacle.h"

namespace dd {

    void DislocationPoint::setCaches() {
        caches.clear();
        caches.reserve(2);
        caches.push_back(ForceCache(FromDislocations::getInstance()));
        caches.push_back(ForceCache(FromFemMock::getInstance()));
    }

    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane, double slipPlanePosition, int burgersSign) :
            Point(slipPlanePosition),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane);
        setCaches();
    }
    DislocationPoint::DislocationPoint(Domain * domain, SlipPlane * sPlane,
                         typename list<Point *>::iterator antecedentIt, double slipPlanePosition, int burgersSign) :
            Point(slipPlanePosition),
            burgersSign(burgersSign) {
        setRegistrations(domain, sPlane, antecedentIt);
        setCaches();
    }

	void DislocationPoint::__pinTo(ObstaclePoint * obstacle, ObstaclePoint * & pinPointer) {
		if(pinPointer == obstacle) { return; }
		if(pinPointer) { DdError::exception("Already pinned."); }
		pinPointer = obstacle;
	}
    
    void DislocationPoint::__unpin(ObstaclePoint * & pinPointer) {
    	if(!pinPointer) { DdError::exception("Not pinned."); }
    	pinPointer = nullptr;
	}
	
	
    bool DislocationPoint::pinned() const {
        return __negativePin || __positivePin;
    }
}
