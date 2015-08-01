#ifndef OOFEMINTERFACE_H_INCLUDED
#define OOFEMINTERFaCE_H_INCLUDED

#include "feminterface.h"

namespace oofem {
    class DDLinearStatic;
}

namespace dd {

	class Domain;

    class OofemInterface : public FemInterface {
#define OOFEMINTERFACE_NAME "OofemInterface"
    private:
        oofem::DDLinearStatic * engModel;
    public:
        OofemInterface(oofem::DDLinearStatic * engModel, Domain * domain = nullptr) :
        	FemInterface(domain), engModel(engModel) { }
    
    
        virtual void addFEMContribution(const Point * point, Vector<2> &force,
                                        Vector<2> &forceGradient, Vector<3> &stress);
        
        virtual string typeName() const { return OOFEMINTERFACE_NAME; }
        static string staticTypeName() { return OOFEMINTERFACE_NAME; }
    };

}

#endif
