#ifndef FROMDISLOCATIONS_H
#define FROMDISLOCATIONS_H

#include "forcefunctor.h"
#include "../point/dislocation.h"

namespace dd {
	
	class FromDislocations : public ForceFunctor {
#define FROMDISLOCATIONS_NAME "FromDislocations"
	public:
		FromDislocations(Point * point) :
			ForceFunctor(point) { }
		virtual ~FromDislocations() { }

		virtual void calculate(Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) {
			point->addForceContribution<DislocationPoint>(force, forceGradient, stress);
		}
	
        virtual string typeName() const { return FROMDISLOCATIONS_NAME; }
        static string staticTypeName() { return FROMDISLOCATIONS_NAME; }
	};

}

#endif
