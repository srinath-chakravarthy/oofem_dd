#include "point/source.h"
#include "point/dislocation.h"
#include "../forcefunctor/fromdislocations.h"
#include "../forcefunctor/fromfem.h"
#include "../forcefunctor/fromfemmock.h"
#include "domain.h"
#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>

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
    		double nu = domain()->getPassionsRatio();
    		double inuc = domain()->getModulus() *getBurgersMagnitude() / (4 * M_PI * (1 - nu * nu) * __strength);
    		
    		double posNegative = slipPlanePosition() - 0.5 * inuc;
    		double posPositive = slipPlanePosition() + 0.5 * inuc;
    		
    		int signNeg = (ratio < 0)? 1 : -1;
    		int signPos = signNeg * -1;
    		
		
    		std::list<Point *> & dislocs = slipPlane()->getContainer("DislocationPoint");
    		
    		auto antecedentIt = dislocs.begin();
    		
    		while(antecedentIt != dislocs.end() && (*antecedentIt)->slipPlanePosition() < posNegative) {
    			antecedentIt++;
    		}
    		
    		if(antecedentIt == dislocs.end() || (*antecedentIt)->slipPlanePosition() > posPositive) {
    			new DislocationPoint(domain(), slipPlane(), antecedentIt, posNegative, signNeg);
    			new DislocationPoint(domain(), slipPlane(), antecedentIt, posPositive, signPos);
    		}
    		std::cout << "Generating dislocatios" << posNegative << " and " << posPositive << "\n";
		__timer = 0.0;
    	}
    }
}
