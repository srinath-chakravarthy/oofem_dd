#include "../oofemlib/timestep.h"

#include "domain.h"
#include "ddcounter.h"
#include "timemanager.h"
#include "dderror.h"

namespace dd {
	TimeManager::TimeManager(Domain * domain, oofem::TimeStep * timeStep = nullptr) :
		domain(domain), timeStep(timeStep) { 
	    if(timeStep != nullptr) {
            updateToNextFemStep(timeStep);	    
	    }
	}
	TimeManager::~TimeManager() { }

	DdCounter * TimeManager::getCurrentCounter() { 
        if(counterHistory.empty()) {
            DdError::exception("TimeManager not initiated.");
        }	   
        return counterHistory.front(); 
	}
	bool TimeManager::isCurrent(DdCounter * counter) { return counter->equals(getCurrentCounter()); }
	
	void TimeManager::updateToNextDdStep() {
		counterHistory.push_front(getCurrentCounter()->constructNextDdStep());
	}
	
	void TimeManager::updateToNextFemStep(oofem::TimeStep * timeStep) {
		counterHistory.push_front(new DdCounter(timeStep));
	}


}
	
