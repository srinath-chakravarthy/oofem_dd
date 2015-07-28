#include "domain.h"
#include "ddcounter.h"
#include "timemanager.h"
#include "dderror.h"

namespace dd {
	TimeManager::TimeManager(Domain * domain, long long femStep) :
		domain(domain) { 
	    counterHistory.push_front(new DdCounter(femStep));
	}
	TimeManager::~TimeManager() { }

	DdCounter * TimeManager::getCurrentCounter() {
        return counterHistory.front(); 
	}
	bool TimeManager::isCurrent(DdCounter * counter) { return counter->equals(getCurrentCounter()); }
	
	void TimeManager::updateToNextDdStep() {
		counterHistory.push_front(getCurrentCounter()->constructNextDdStep());
	}
	
	void TimeManager::updateToNextFemStep() {
		counterHistory.push_front(getCurrentCounter()->constructNextDdStep());
	}


}
	
