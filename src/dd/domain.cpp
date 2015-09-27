#include "domain.h"
#include "slipsystem.h"
#include "point.h"
#include "vector.h"
#include "point/dislocation.h"
#include "point/source.h"
#include "feminterface/feminterface.h"

#include "../sm/EngineeringModels/DDlinearstatic.h"
#include "spatiallocalizer.h"
#include "element.h"
#include "../oofemlib/domain.h"
#include "gausspoint.h"
#include "../sm/Elements/structuralelement.h"

#include <iostream>

namespace dd {

    Domain::Domain(const double & propModulus, const double & propPassionsRatio, FemInterface * femInterface) :
        propModulus(propModulus), propPassionsRatio(propPassionsRatio),
        femInterface(femInterface) { 
	if (femInterface){
	  femInterface->setDomain(this);    
	}
    }
        
    void Domain::updateForceCaches()  {
        for(auto keyValue : containers) {
            for(Point * point : keyValue.second) {
                point->updateCaches();
            }
        }
    }

        
    void Domain::addFEMContribution(const Point * point, Vector<2> &force,
                            Vector<2> &forceGradient, Vector<3> &stress) const {
        femInterface->addFEMContribution(point, force, forceGradient, stress);
    }
    
    void Domain::moveDislocations(double dt, double b) {
        for(auto slipSystem : sSystems) {
           for(auto slipPlane : slipSystem->getSlipPlanes()) {
               slipPlane->moveDislocations(dt, b);
           }
        } 
    }
    
    void Domain::setFemInterface(FemInterface * femInterface) { this->femInterface = femInterface; }
    FemInterface * Domain::getFemInterface() const { return this->femInterface; }
    double Domain::getModulus() const { return propModulus; }
    double Domain::getPassionsRatio() const { return propPassionsRatio; }

    void Domain::addSlipSystem(SlipSystem * slipSystem) {
        sSystems.push_back(slipSystem);
    }
}

