#include "domain.h"
#include "slipsystem.h"
#include "point.h"
#include "vector.h"
#include "point/dislocation.h"
#include "point/source.h"
#include <cmath>
#include "complex.h"
#include <string>

namespace dd {

    Domain::Domain(const double & propModulus, const double & propPassionsRatio) :
        propModulus(propModulus), propPassionsRatio(propPassionsRatio) { }

    Domain::Domain() : Domain::Domain(0, 0) { }

    double Domain::getModulus() const { return propModulus; }
    double Domain::getPassionsRatio() const { return propPassionsRatio; }

    void Domain::addSlipSystem(SlipSystem * slipSystem) {
        sSystems.push_back(slipSystem);
    }
}

