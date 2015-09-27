#ifndef FROMFEM_H
#define FROMFEM_H

#include "forcefunctor.h"

namespace dd {
	
    class FromFem : public ForceFunctor {
#define FROMFEM_NAME "FromFem"
    private:
        static FromFem * instance;
        FromFem() { }
    protected:
        virtual ~FromFem() { }
    public:
        
        virtual void calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress);
        static FromFem * getInstance();
            
        virtual string typeName() const { return FROMFEM_NAME; }
        static string staticTypeName() { return FROMFEM_NAME; }
    };
}

#endif
