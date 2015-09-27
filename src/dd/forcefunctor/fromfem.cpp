#include "fromfem.h"
#include "point.h"

namespace dd {
    
        FromFem * FromFem::instance = nullptr;
	
	FromFem * FromFem::getInstance() { 
            if(instance == nullptr) {
                instance = new FromFem();
            }
            return instance;
	}
	
	void FromFem::calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) {
		point->domain()->addFEMContribution(point, force, forceGradient, stress);
	}

}
