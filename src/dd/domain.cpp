#include "domain.h"
#include "slipsystem.h"
#include "point.h"
#include "vector.h"
#include "point/dislocation.h"
#include "point/source.h"
#include <cmath>
#include "complex.h"
#include <string>

#include "../sm/EngineeringModels/DDlinearstatic.h"
#include "spatiallocalizer.h"
#include "element.h"
#include "../oofemlib/domain.h"
#include "gausspoint.h"
#include "../sm/Elements/structuralelement.h"

namespace dd {

    Domain::Domain(oofem::DDLinearStatic * engModel, const double & propModulus, const double & propPassionsRatio) :
        propModulus(propModulus), propPassionsRatio(propPassionsRatio),
        engModel(engModel) { }
        
    void Domain::addFEMContribution(const Point * point, Vector<2> &force,
                            Vector<2> &forceGradient, Vector<3> &stress) const {
        for(int i = 1; i <= engModel->giveNumberOfDomains(); i++) {
            oofem::FloatArray localCoordinates, strainElem, stressElem;
            oofem::Element * e = engModel->giveDomain(i)->giveSpatialLocalizer()->giveElementContainingPoint(point->getLocation());
            e->computeLocalCoordinates(localCoordinates, point->getLocation());
               
            oofem::GaussPoint gp(e->giveDefaultIntegrationRulePtr(), -1,
                                 localCoordinates, 1, e->giveMaterialMode());
            oofem::StructuralElement * se = static_cast<oofem::StructuralElement *>(e);
            se->computeStrainVector(strainElem, &gp, engModel->giveCurrentStep());
            se->computeStressVector(stressElem, strainElem, &gp, engModel->giveCurrentStep());
            
            double cos2i = 1;
            double sin2i = 1;
            
            force += point->getBurgersMagnitude() * point->getBurgersSign() *
                     (((stressElem[1] - stressElem[2]) * cos2i) + stressElem[3] * sin2i / 2);
                
        }
    }

    double Domain::getModulus() const { return propModulus; }
    double Domain::getPassionsRatio() const { return propPassionsRatio; }

    void Domain::addSlipSystem(SlipSystem * slipSystem) {
        sSystems.push_back(slipSystem);
    }
}

