#include "fromfemmock.h"
#include "point.h"

namespace dd {

    FromFemMock * FromFemMock::instance = nullptr;
	
	FromFemMock * FromFemMock::getInstance() { 
		if(instance == nullptr) {
			instance = new FromFemMock();
		}
		return instance;
	}
	
	void FromFemMock::calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) {
		force = {1, 1};
		forceGradient = {0, 0};
		stress = {0, 0};
	}

}
