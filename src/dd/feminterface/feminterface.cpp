#include "feminterface.h"
#include "../domain.h"
#include "../point.h"
#include "../point/dislocation.h"
#include "../slipsystem.h"
#include "../slipplane.h"
#include "../complex.h"
#define _USE_MATH_DEFINES
#include <cmath>

namespace dd {

	FemInterface::FemInterface(Domain * domain)
		: domain(domain) {
		domain->setFemInterface(this);
	}
    
    void FemInterface::giveNodalBcContribution(const Vector<2> & coords,
                                         Vector<2> &bcContribution) {
        for(auto point : domain->getContainer<DislocationPoint>()) {
            double e = domain->getModulus();
            double nu = domain->getPassionsRatio();
            double factor = e/(1 + nu);
            double kappa = 3 - 4 * nu;
            double fac2 = e/(8 * M_PI * (1 - nu * nu));
            double cutOff = 2 * abs(point->getBurgersMagnitude());
            Complex z0(coords);
            Complex b(point->getBurgersVector());
            Complex z(point->getLocation());
            Complex slip(point->getSlipPlane()->getSlipSystem()->getDirection());
            Complex uu;
            Complex dz = z0 - z;
            double zd = dz.abs();
            
            if(zd > cutOff) {
                Complex phi = -Complex::i * fac2 * b * log(slip.conjugate() * dz);
                Complex xsi = Complex::i * fac2 * b.conjugate() * log(slip.conjugate() * dz);
                Complex dphi = -Complex::i * b * fac2 / dz;
                uu = (kappa * phi - dz * dphi.conjugate() - xsi.conjugate()) / factor;
            }
            
            bcContribution += Vector<2>({uu.real(), uu.imag()});
        }                                    
    }
}
