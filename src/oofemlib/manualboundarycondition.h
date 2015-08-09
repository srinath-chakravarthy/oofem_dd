#ifndef manualboundarycondition_h
#define manualboundarycondition_h

#include "boundarycondition.h"
#include "floatarray.h"
#include "bctype.h"
#include "valuemodetype.h"

/**
 * @name Dirichlet boundary condition.
 *
 */
//@{
#define _IFT_ManualBoundaryCondition_Name "manualboundarycondition"
//@}

namespace oofem {

class OOFEM_EXPORT ManualBoundaryCondition : public BoundaryCondition {
protected:
    FloatArray manualValues;
public:

    ManualBoundaryCondition(int i, Domain * d) : BoundaryCondition(i, d)
    { }
    /// Destructor
    virtual ~ManualBoundaryCondition() { }

    virtual double getManualValue(Dof * dof) const;
    virtual void setManualValue(Dof *dof, const double & val);
    virtual void addManualValue(Dof *dof, const double & val);
    virtual double give(Dof *dof, ValueModeType mode, TimeStep *tStep);
    virtual double give(Dof *dof, ValueModeType mode, double time);

    virtual IRResultType initializeFrom(InputRecord *ir);
    virtual void giveInputRecord(DynamicInputRecord &input);
    
    virtual const char *giveClassName() const { return "ManualBoundaryCondition"; }
    virtual const char *giveInputRecordName() const { return _IFT_ManualBoundaryCondition_Name; } 
};


}

#endif
