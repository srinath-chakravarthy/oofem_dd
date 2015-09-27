#ifndef FORCEFUNCTOR_H
#define FORCEFUNCTOR_H

#include "ddobject.h"
#include "vector.h"

namespace dd {
      
    class Point;
      
    class ForceFunctor : public DdObject {
#define FORCEFUNCTOR_NAME "ForceFunctor" 	
    protected:     
        virtual ~ForceFunctor() { }
    public:
	virtual void calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) = 0;
	virtual string typeName() const { return FORCEFUNCTOR_NAME; }
	static string staticTypeName() { return FORCEFUNCTOR_NAME; }
    };

}

#endif
