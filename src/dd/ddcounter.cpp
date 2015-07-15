#include "ddcounter.h"
#include "../oofemlib/timestep.h"

namespace dd {
	DdCounter::DdCounter(long long count, oofem::TimeStep * timeStep) :
		count(count), timeStep(timeStep) { }
	DdCounter::DdCounter(oofem::TimeStep * timeStep) :
		DdCounter(0, timeStep) { }
   
   	bool DdCounter::equals(const DdObject * other) const {
   		const DdCounter * asCounter = dynamic_cast<const DdCounter *>(other);
   		if(asCounter == nullptr) { return false; }
   		return count == asCounter->getCount() && timeStep == asCounter->getTimeStep();
	}

    long long DdCounter::getCount() const { return count; }
    oofem::TimeStep * DdCounter::getTimeStep() const { return timeStep; }
	
	DdCounter * DdCounter::constructNextDdStep() const {
		return new DdCounter(count + 1, timeStep);
	}
    

}
