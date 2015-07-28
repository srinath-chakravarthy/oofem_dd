#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include "ddobject.h"
#include <list>

namespace dd {

    class Domain;
    class DdCounter;
  
    class TimeManager : public DdObject {
#define TIMEMANAGER_NAME "TimeManager";
        Domain * domain;
       	std::list<DdCounter *> counterHistory;
    public:
		TimeManager(Domain * domain, long long femStep = 0);
		virtual ~TimeManager();
		
		DdCounter * getCurrentCounter();
		bool isCurrent(DdCounter * counter);	
		void updateToNextDdStep();
		void updateToNextFemStep();

      
        virtual string typeName() const { return TIMEMANAGER_NAME; }
        static string staticTypeName() { return TIMEMANAGER_NAME; }
    };
}

#endif
