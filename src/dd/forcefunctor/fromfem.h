#ifndef FROMFEM_H
#define FROMFEM_H

#include "forcefunctor.h"
#include "../point/dislocation.h"

namespace dd {
	
	class FromFem : public ForceFunctor {
#define FROMFEM_NAME "FromFem"
	public:
		FromFem(Point * point) :
			ForceFunctor(point) { }
		virtual ~FromFem() { }

		virtual void calculate(Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) {
			point->getDomain()->addFEMContribution(point, force, forceGradient, stress);
		}
	
        virtual string typeName() const { return FROMFEM_NAME; }
        static string staticTypeName() { return FROMFEM_NAME; }
	};

}

#endif
