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
	      double slope = 500e-6/ point->domain()->dtNomax;
	      double bmag = point->getBurgersMagnitude() * point->getBurgersSign();
	      int t = point->domain()->dtNo;
	      double sig = double(t) * slope;
	      double f = sig * bmag;
	      //f = 0.0;
	      force = {f, 0.};
	      forceGradient = {0., 0.};
	      //sig = 0.0;
	      stress = {0., 0., sig};
	}

}
