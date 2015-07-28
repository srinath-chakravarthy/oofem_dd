#include "ddcounter.h"

namespace dd {
	DdCounter::DdCounter(long long ddStep, long long femStep) :
		ddStep(ddStep), femStep(femStep) { }
	DdCounter::DdCounter(long long femStep) :
		DdCounter(0, femStep) { }
   
   	bool DdCounter::equals(const DdObject * other) const {
   		const DdCounter * asCounter = dynamic_cast<const DdCounter *>(other);
   		if(asCounter == nullptr) { return false; }
   		return ddStep == asCounter->getDdStep() && femStep == asCounter->getFemStep();
	}

    long long DdCounter::getDdStep() const { return ddStep; }
    long long DdCounter::getFemStep() const { return femStep; }
	
	DdCounter * DdCounter::constructNextDdStep() const {
		return new DdCounter(ddStep + 1, femStep);
	}
    DdCounter * DdCounter::constructNextFemStep() const {
        return new DdCounter(0, femStep + 1);    
    }
    

}
