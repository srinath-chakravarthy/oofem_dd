#include "manualboundarycondition.h"
#include "dof.h"
#include "timestep.h"
#include "function.h"
#include "verbose.h"
#include "dynamicinputrecord.h"
#include "dof.h"
#include "classfactory.h"
#include "contextioerr.h"

namespace oofem {
REGISTER_BoundaryCondition(ManualBoundaryCondition);

double
ManualBoundaryCondition :: getManualValue(Dof * dof) const { 
	return manualValues.at(this->dofs.findFirstIndexOf(dof->giveDofID())); 
}
void 
ManualBoundaryCondition :: setManualValue(Dof * dof, const double & val) { 
	manualValues.at(this->dofs.findFirstIndexOf(dof->giveDofID())) = val; 
}
void 
ManualBoundaryCondition :: addManualValue(Dof * dof, const double & val) { 
	manualValues.at(this->dofs.findFirstIndexOf(dof->giveDofID())) += val; 
}

double ManualBoundaryCondition :: give(Dof *dof, ValueModeType mode, TimeStep *tStep){
    return this->give(dof, mode, tStep->giveIntrinsicTime());
}


double ManualBoundaryCondition :: give(Dof *dof, ValueModeType mode, double time)
{
    double factor = 0;
    if ( mode == VM_Total ) {
        factor = this->giveTimeFunction()->evaluateAtTime(time);
    } else {
        OOFEM_ERROR("Should only be called on VM_Total.");
    }
    int index = this->dofs.findFirstIndexOf( dof->giveDofID());
    if ( !index ) {
        index = 1;
    }

    return this->values.at(index) * factor + manualValues.at(index);
}

IRResultType
ManualBoundaryCondition :: initializeFrom(InputRecord *ir) {
    IRResultType result;                // Required by IR_GIVE_FIELD macro

    result = BoundaryCondition :: initializeFrom(ir);
    if ( result != IRRT_OK ) {
        return result;
    }
    
    manualValues.resize( (this->dofs.giveSize())? this->dofs.giveSize() : 1);
    manualValues.zero();

    return IRRT_OK;
}

void
ManualBoundaryCondition :: giveInputRecord(DynamicInputRecord &input) {
    BoundaryCondition :: giveInputRecord(input);
}


}
