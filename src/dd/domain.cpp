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

namespace dd {

    Domain::Domain(const double & propModulus, const double & propPassionsRatio, FemInterface * femInterface) :
        propModulus(propModulus), propPassionsRatio(propPassionsRatio),
        femInterface(femInterface) { 
    	femInterface->setDomain(this);    
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
        /*
        for(int i = 1; i <= engModel->giveNumberOfDomains(); i++) {
            oofem::FloatArray localCoordinates, strainElem, stressElem;
            oofem::Element * e = engModel->giveDomain(i)->giveSpatialLocalizer()->giveElementContainingPoint(point->getLocation());
            e->computeLocalCoordinates(localCoordinates, point->getLocation());
               
            oofem::GaussPoint gp(e->giveDefaultIntegrationRulePtr(), -1,
                                 localCoordinates, 1, e->giveMaterialMode());
            oofem::StructuralElement * se = static_cast<oofem::StructuralElement *>(e);
            se->computeStrainVector(strainElem, &gp, engModel->giveCurrentStep());
            se->computeStressVector(stressElem, strainElem, &gp, engModel->giveCurrentStep());
                        
            force += point->getBurgersMagnitude() * point->getBurgersSign() *
                     (((stressElem[1] - stressElem[2]) * point->getSlipPlane()->getCos()) + 
                       stressElem[3] * point->getSlipPlane()->getSin() / 2);
                
        }
        */
    }
    
    void Domain::setFemInterface(FemInterface * femInterface) { this->femInterface = femInterface; }
    FemInterface * Domain::getFemInterface() const { return this->femInterface; }
    double Domain::getModulus() const { return propModulus; }
    double Domain::getPassionsRatio() const { return propPassionsRatio; }

    void Domain::addSlipSystem(SlipSystem * slipSystem) {
        sSystems.push_back(slipSystem);
    }
}

