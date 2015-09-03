
#include "point.h"
#include "point/obstacle.h"
#include "point/dislocation.h"

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
            __negativePinned = nullptr;
        }
        else if(releasePositive) {
            __positivePinned->updateLocation(__positivePinned->getSlipPlanePosition() - 4 * __positivePinned->getBurgersMagnitude());
            __positivePinned = nullptr;        
        }
    }

}


