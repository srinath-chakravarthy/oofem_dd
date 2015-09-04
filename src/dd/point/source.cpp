#include "point/source.h"
#include "point/dislocation.h"
#include "../forcefunctor/fromdislocations.h"
#include "../forcefunctor/fromfem.h"
#include "../forcefunctor/fromfemmock.h"
#include "domain.h"
#define _USE_MATH_DEFINES
#include <cmath>

namespace dd {
    void SourcePoint::setCaches() {
        caches.clear();
        caches.reserve(2);
        caches.push_back(ForceCache(FromDislocations::getInstance()));
        caches.push_back(ForceCache(FromFemMock::getInstance()));
    }
    
    void SourcePoint::spawn(const double & dt, const double & tnuc) {
    	double ratio = cachedForce().abs() / getBurgersMagnitude();
    	if(ratio > __strength) {
    		__timer += dt;
    	}
    	
    	if(__timer > tnuc) {
    		double nu = getDomain()->getPassionsRatio();
    		double inuc = getDomain()->getModulus() / (4 * M_PI * (1 - nu * nu) * __strength);
    		
    		double posNegative = getSlipPlanePosition() - 0.5 * inuc;
    		double posPositive = getSlipPlanePosition() + 0.5 * inuc;
    		
    		int signNeg = (ratio < 0)? 1 : -1;
    		int signPos = signNeg * -1;
    		
    		std::list<Point *> & dislocs = getSlipPlane()->getContainer("DislocationPoint");
    		
    		auto itN = dislocs.begin(), itP = dislocs.begin();
    		
    		while(itN != dislocs.end() && (*itN)->getSlipPlanePosition() < posNegative) {
    			itN++;
    		}
    		while(itP != dislocs.end() && (*itP)->getSlipPlanePosition() < posPositive) {
    			itP++;
    		}
    		
    		new DislocationPoint(getDomain(), getSlipPlane(), itN, posNegative, signNeg);
    		new DislocationPoint(getDomain(), getSlipPlane(), itP, posPositive, signPos);
    		
    	}
    }
}
