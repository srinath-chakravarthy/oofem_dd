
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
        bool releasePositive = (__positivePinned && releaseNegative) || checkRelease(__positivePinned);
        
        if(releaseNegative && releasePositive) {
            delete __negativePinned;
            delete __positivePinned;
            __negativePinned = nullptr;
            __positivePinned = nullptr;
        }
        else if(releaseNegative) {
            __negativePinned->updateLocation(__negativePinned->getSlipPlanePosition() + 4 * __negativePinned->getBurgersMagnitude());
            __negativePinned->unpin();
            __negativePinned = nullptr;
        }
        else if(releasePositive) {
            __positivePinned->updateLocation(__positivePinned->getSlipPlanePosition() - 4 * __positivePinned->getBurgersMagnitude());
            __positivePinned->unpin();
            __positivePinned = nullptr;        
        }
    }
    
    void ObstaclePoint::pin(DislocationPoint * point) {
        if(point->getBurgersSign() == 1) {
            if(__negativePinned == point) { return; }
            if(__negativePinned) { DdError::exception("Obstacle has a dislocation pinned at the negative side."); }
            __negativePinned = point;
            point->pinTo(this);
        }
        else {
            if(__positivePinned == point) { return; }
            if(__positivePinned) { DdError::exception("Obstacle has a dislocation pinned at the positive side."); }
            __positivePinned = point;
            point->pinTo(this);
        }
    }

}


