#include "fromfemmock.h"
#include "point.h"
#include "../domain.h"

namespace dd {

    FromFemMock * FromFemMock::instance = nullptr;
	
	FromFemMock * FromFemMock::getInstance() { 
	    if(instance == nullptr) {
		instance = new FromFemMock();
	    }
	    return instance;
	}
	
	void FromFemMock::calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) {
	      double slope = 500e-3/ point->getDomain()->dtNomax;
	      double bmag = point->getBurgersMagnitude();
	      int t = point->getDomain()->dtNo;
	      double sig = double(t) * slope;
	      double f = sig * bmag; 
	      force = {f, 0.};
	      forceGradient = {0., 0.};
	      stress = {0., 0., sig};
	}

}
