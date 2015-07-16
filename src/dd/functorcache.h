#ifndef FUNCTORCACHE_H
#define FUNCTORCACHE_H

#include "forcefunctor/forcefunctor.h"

namespace dd {

	class ForceFunctor;

	struct ForceData {
		Vector<2> force;
		Vector<2> forceGradient;
		Vector<2> stress;
	}

	class FunctorCache : public DdObject {
#define FUNCTORCACHE_NAME "FunctorCache"
	private:
		std::map<ForceFunctor, ForceData> cache;
	public:
		
		ForceData get()
		void update();
		
	
        virtual string typeName() const { return FUNCTORCACHE_NAME; }
        static string staticTypeName() { return FUNCTORCACHE_NAME; }
	};

}

#endif
