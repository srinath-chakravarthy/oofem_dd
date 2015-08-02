#ifndef FEMINTERFACE_H_INCLUDED
#define FEMINTERFaCE_H_INCLUDED

#include "../ddobject.h"
#include "../vector.h"
#include <vector>

namespace dd {
    
    class Point;
	class Domain;

    class FemInterface : public DdObject {
#define FEMINTERFACE_NAME "FemInterface"        	
    private:
		Domain * domain;
    public:
    	FemInterface(Domain * domain);
    	virtual ~FemInterface() { }
    	virtual void setDomain(Domain * domain) { this->domain = domain; }
        virtual Domain * getDomain() { return this->domain; }
        virtual void addFEMContribution(const Point * point, Vector<2> &force,
                                        Vector<2> &forceGradient, Vector<3> &stress) = 0;
        virtual void giveNodalBcContribution(const Vector<2> & coords,
                                             Vector<2> &bcContribution);
                                        
        virtual string typeName() const { return FEMINTERFACE_NAME; }
        static string staticTypeName() { return FEMINTERFACE_NAME; }
    };

}



#endif
