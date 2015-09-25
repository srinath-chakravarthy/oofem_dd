#include "oofeminterface.h"
#include "../domain.h"
#include "point.h"
#include "../vector.h"

#include "../../sm/EngineeringModels/DDlinearstatic.h"
#include "spatiallocalizer.h"
#include "element.h"
#include "../../oofemlib/domain.h"
#include "gausspoint.h"
#include "../../sm/Elements/structuralelement.h"
#include "generalboundarycondition.h"
#include "node.h"


namespace dd {

	void OofemInterface::addFEMContribution(const Point * point, Vector<2> &force,
                                        Vector<2> &forceGradient, Vector<3> &stress) {
    	for(int i = 1; i <= engModel->giveNumberOfDomains(); i++) {
            oofem::FloatArray localCoordinates, strainElem, stressElem;
            oofem::Element * e = engModel->giveDomain(i)->giveSpatialLocalizer()->giveElementContainingPoint(point->location());
            e->computeLocalCoordinates(localCoordinates, point->location());
               
            oofem::GaussPoint gp(e->giveDefaultIntegrationRulePtr(), -1,
                                 localCoordinates, 1, e->giveMaterialMode());
            oofem::StructuralElement * se = static_cast<oofem::StructuralElement *>(e);
            se->computeStrainVector(strainElem, &gp, engModel->giveCurrentStep());
            se->computeStressVector(stressElem, strainElem, &gp, engModel->giveCurrentStep());
                        
            force += point->getBurgersMagnitude() * point->getBurgersSign() *
                     (((stressElem[1] - stressElem[2]) * point->slipPlane()->getCos()) + 
                       stressElem[3] * point->slipPlane()->getSin() / 2);
                
        } 
    }
    
    void OofemInterface::giveNodalBcContribution(oofem::Node * node, Vector<2> &bcContribution) {
        FemInterface::giveNodalBcContribution({node->giveCoordinates()->at(1), node->giveCoordinates()->at(2)}, bcContribution);
    }
    void OofemInterface::giveNodalBcContribution(oofem::GeneralBoundaryCondition * bc, Vector<2> &bcContribution) {
        if(bc->giveType() == oofem::DirichletBT) {
            for(int dofManagerNo = 1; dofManagerNo <= bc->giveNumberOfInternalDofManagers(); dofManagerNo++) {
            	oofem::Node * node = dynamic_cast<oofem::Node *>(bc->giveInternalDofManager(dofManagerNo));
            	if(node == nullptr) { continue; }
            	giveNodalBcContribution(node, bcContribution);
            }
        }
    }

}
