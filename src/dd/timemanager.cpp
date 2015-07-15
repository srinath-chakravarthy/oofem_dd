#include "../oofemlib/timestep.h"

#include "domain.h"
#include "ddcounter.h"
#include "timemanager.h"

namespace dd {
	TimeManager::TimeManager(Domain * domain, oofem::TimeStep * timeStep) :
		domain(domain), timeStep(timeStep) { }
	TimeManager::~TimeManager() { }

	DdCounter * TimeManager::getCurrentCounter() { return counterHistory.front(); }
	bool TimeManager::isCurrent(DdCounter * counter) { return counter->equals(getCurrentCounter()); }
	
	void TimeManager::updateToNextDdStep() {
		counterHistory.push_front(getCurrentCounter()->constructNextDdStep());
	}
	
	void TimeManager::updateToNextFemStep(oofem::TimeStep * timeStep) {
		counterHistory.push_front(new DdCounter(timeStep));
	}


}
	
