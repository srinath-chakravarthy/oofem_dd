#include "oofeminterface.h"
#include "domain.h"
#include "point.h"

#include "../../sm/EngineeringModels/DDlinearstatic.h"
#include "spatiallocalizer.h"
#include "element.h"
#include "../../oofemlib/domain.h"
#include "gausspoint.h"
#include "../../sm/Elements/structuralelement.h"


namespace dd {

	void OofemInterface::addFEMContribution(const Point * point, Vector<2> &force,
                                        Vector<2> &forceGradient, Vector<3> &stress) {
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
    }

}
