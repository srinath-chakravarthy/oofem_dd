#ifndef FROMFEMMOCK_H
#define FROMFEMMOCK_H

#include "forcefunctor.h"

namespace dd {
	
	class FromFemMock : public ForceFunctor {
#define FROMFEMMOCK_NAME "FromFemMOCK"
	    static FromFemMock * instance;
	    FromFemMock() { }
	public:
	    virtual ~FromFemMock() { }

		virtual void calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress);
		static FromFemMock * getInstance();
		
        virtual string typeName() const { return FROMFEMMOCK_NAME; }
        static string staticTypeName() { return FROMFEMMOCK_NAME; }
	};
}

#endif
