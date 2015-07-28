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
		long long ddStep;
		long long femStep;
    public:
    	DdCounter(long long ddStep, long long femStep);
    	DdCounter(long long femStep);
    	
    	long long getDdStep() const;
    	long long getFemStep() const;
    	virtual bool equals(const DdObject * other) const;
    	DdCounter * constructNextDdStep() const;
    	DdCounter * constructNextFemStep() const;
    
      
        virtual string typeName() const { return DDCOUNTER_NAME; }
        static string staticTypeName() { return DDCOUNTER_NAME; }
    };
}

#endif
