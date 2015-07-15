#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "ddobject.h"
#include <list>

namespace oofem {
	class TimeStep;
}


namespace dd {

    class Domain;
    class DdCounter;
  
    class TimeManager : public DdObject {
#define TIMEMANAGER_NAME "TimeManager";
        Domain * domain;
       	oofem::TimeStep * timeStep;
       	std::list<DdCounter *> counterHistory;
    public:
		TimeManager(Domain * domain, oofem::TimeStep * timeStep);
		virtual ~TimeManager();
		
		DdCounter * getCurrentCounter();
		bool isCurrent(DdCounter * counter);	
		void updateToNextDdStep();
		void updateToNextFemStep(oofem::TimeStep * timeStep);

      
        virtual string typeName() const { return TIMEMANAGER_NAME; }
        static string staticTypeName() { return TIMEMANAGER_NAME; }
    };
}

#endif
