#ifndef FORCEFUNCTOR_H
#define FORCEFUNCTOR_H

#include "ddobject.h"
#include "vector.h"

namespace dd {
	
	class Point;
	
	class ForceFunctor : public DdObject {
#define FORCEFUNCTOR_NAME "ForceFunctor"
	protected:
		Point * point; 	
	public:	
		ForceFunctor(Point * point) :
			point(point) { }
		virtual ~ForceFunctor() { }
		virtual void calculate(Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) = 0;
		virtual bool equals(const DdObject * other) {
			const ForceFunctor * asFunctor = dynamic_cast<const ForceFunctor *>(other);
			if(asFunctor == nullptr) { return false; }
			return asFunctor->typeName() == typeName();
		}
        virtual string typeName() const { return FORCEFUNCTOR_NAME; }
        static string staticTypeName() { return FORCEFUNCTOR_NAME; }
	};

}

#endif
