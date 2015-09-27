#ifndef FROMDISLOCATIONS_H
#define FROMDISLOCATIONS_H

#include "forcefunctor.h"

namespace dd {
	
	class FromDislocations : public ForceFunctor {
#define FROMDISLOCATIONS_NAME "FromDislocations"
    private:
        static FromDislocations * instance;
        FromDislocations() { }
    protected:
        virtual ~FromDislocations() { }
    public:

        virtual void calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress);
		
	static FromDislocations * getInstance();
			
        virtual string typeName() const { return FROMDISLOCATIONS_NAME; }
        static string staticTypeName() { return FROMDISLOCATIONS_NAME; }
	};

}

#endif
