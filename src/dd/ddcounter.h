#ifndef DDCOUNTER_H
#define DDCOUNTER_H

#include "ddobject.h"

namespace oofem {
	class TimeStep;
}


namespace dd {

  
    class DdCounter : public DdObject {
#define DDCOUNTER_NAME "DdCounter"
	private:
		long long count;
		oofem::TimeStep * timeStep;
    public:
    	DdCounter(long long count, oofem::TimeStep * timeStep);
    	DdCounter(oofem::TimeStep * timeStep);
    	
    	long long getCount() const;
    	oofem::TimeStep * getTimeStep() const;
    	virtual bool equals(const DdObject * other) const;
    	DdCounter * constructNextDdStep() const;
    
      
        virtual string typeName() const { return DDCOUNTER_NAME; }
        static string staticTypeName() { return DDCOUNTER_NAME; }
    };
}

#endif
