
#include "point.h"
#include "point/obstacle.h"
#include "point/dislocation.h"
#include "dderror.h"

namespace dd {
        
    bool ObstaclePoint::checkRelease(const DislocationPoint * point) const {
        return point->cachedForce().abs() / point->getBurgersMagnitude() > strength();
    }
    
    void ObstaclePoint::release() {
        bool releaseNegative = __negativePinned && checkRelease(__negativePinned);
        bool releasePositive = __positivePinned && checkRelease(__positivePinned);
	
	if((releaseNegative && __positivePinned) ||
	  (releasePositive && __negativePinned)) {
	  DdError::exception("Pin release is not symmetric.\n");
	}
        
        if(releaseNegative && releasePositive) {
            delete __negativePinned;
            delete __positivePinned;
            __negativePinned = nullptr;
            __positivePinned = nullptr;
        }
        else if(releaseNegative) {
            __negativePinned->updateLocation(__negativePinned->slipPlanePosition() + 4 * __negativePinned->getBurgersMagnitude());
            __negativePinned->positiveUnpin();
            __negativePinned = nullptr;
        }
        else if(releasePositive) {
            __positivePinned->updateLocation(__positivePinned->slipPlanePosition() - 4 * __positivePinned->getBurgersMagnitude());
            __positivePinned->negativeUnpin();
            __positivePinned = nullptr;        
        }
    }
    
    void ObstaclePoint::pin(DislocationPoint * point) {
        if(point->slipPlanePosition() < slipPlanePosition()) {
            if(__negativePinned == point) { return; }
            if(__negativePinned) {
                DdError::exception("Obstacle has a dislocation pinned at the negative side."); 
            }
            __negativePinned = point;
            point->positivePinTo(this);
        }
        else {
            if(__positivePinned == point) { return; }
            if(__positivePinned) { DdError::exception("Obstacle has a dislocation pinned at the positive side."); }
            __positivePinned = point;
            point->negativePinTo(this);
        }
    }

}


