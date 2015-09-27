#include "fromdislocations.h"
#include "point.h"
#include "../point/dislocation.h"

namespace dd {
    FromDislocations * FromDislocations::instance = nullptr;
  
    FromDislocations * FromDislocations::getInstance() {
        if(instance == nullptr) {
            instance = new FromDislocations();
	}
	return instance;
      }
      
      void FromDislocations::calculate(Point * point, Vector<2> & force, Vector<2> & forceGradient, Vector<3> stress) {
	      point->addForceContribution<DislocationPoint>(force, forceGradient, stress);
      }
      
}
