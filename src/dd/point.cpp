#include "point.h"
#include "dderror.h"
#include "vector.h"
#include "domain.h"
#include "complex.h"
#include "forcefunctor/fromfem.h"
#include "dderror.h"
#include <cmath>
namespace dd {
    double Point::getBurgersMagnitude() const {
        return getSlipPlane()->getBurgersMagnitude();
    }

    int Point::getBurgersSign() const { return 1; }

    Vector<2> Point::getBurgersVector() const {
        return getSlipPlane()->getBurgersVector();
    }

    void Point::addForceContribution(const Point * & p, Vector<2> & force,
                                     Vector<2> & v2, Vector<3> & stress) {
	double e = getDomain()->getModulus();
	double nu = getDomain()->getPassionsRatio();
	double mu = e / (2. * (1. + nu));
        double factor = mu / (4. * M_PI * (1. - nu));

        double cutOff = 2 * std::abs(this->getBurgersMagnitude());
        Complex b(this->getBurgersVector());
        Complex z(this->getLocation());
	Complex thetaz(this->getSlipPlane()->getCos(), -this->getSlipPlane()->getSin());

        double sig11 = 0;
        double sig22 = 0;
        double sig12 = 0;
        double sig11z = 0;
        double sig22z = 0;
        double sig12z = 0;

        Complex zSource(p->getLocation());
        Complex dz = z - zSource;
        double zd = dz.abs();
	

        if(zd > cutOff) {
            Complex phi1 = -1. * Complex::i * factor * b / dz;
            Complex phi11 = Complex::i * factor * b / (dz * dz);
            Complex phi2 = Complex::i * factor * b.conjugate() / dz;

            Complex tmp1 = 2. * Complex(phi1 * phi1.conjugate());
            Complex tmp2 = -2. * Complex(dz * phi11.conjugate() * phi2.conjugate());

            sig11 += Complex(0.5 * (tmp1 + tmp2)).real();
            sig22 += Complex(0.5 * (tmp1 - tmp2)).real();
            sig12 += Complex(.5 * tmp2).imag();

            Complex phi1z = Complex::i * factor * b / (dz * dz) * thetaz;
            Complex phi11z = -2. * Complex::i * factor * b / (dz * dz * dz) * thetaz;
            Complex phi2z = -1. * factor * b.conjugate() / (dz * dz) * thetaz;
            Complex tmp1z = 2. * (phi1z + phi1z.conjugate());
            Complex tmp2z = -2. * (dz * phi11z.conjugate() + phi2z.conjugate());

            sig11z += Complex(.5 * (tmp1z + tmp2z)).real();
            sig22z += Complex(.5 * (tmp1z - tmp2z)).real();
            sig12z += Complex(.5 * tmp2z).imag();
        }
        else {
	  Complex th;
	  if (zd > 1.e-4) { 
	    if (dz.real() == 0.){
	       th = Complex(0., ::asin(this->getSlipPlane()->getSin()));
	    }
	    else {
	      th = Complex(0., dz.log().imag());
	    }
	    Complex zstar = cutOff * th.exp();
	    Complex phi1 = -1. * Complex::i * factor * b / zstar;
	    Complex phi11 = 1. *Complex::i * factor * b / (zstar*zstar);
	    Complex phi2 = 1. * Complex::i * factor * b.conjugate() / zstar;

	    Complex tmp1 = 2. * Complex(phi1 * phi1.conjugate());
            Complex tmp2 = -2. * Complex(zstar * phi11.conjugate() * phi2.conjugate());
	    
            sig11 += Complex(0.5 * (tmp1 + tmp2)).real();
            sig22 += Complex(0.5 * (tmp1 - tmp2)).real();
            sig12 += Complex(.5 * tmp2).imag();
    
	  }
	}

        double cos2i = ::cos(2. * this->getSlipPlane()->getAngle());
        double sin2i = ::sin(2. * this->getSlipPlane()->getAngle());

        double bi = std::abs(getSlipPlane()->getBurgersMagnitude()) * getBurgersSign() *
                    ((sig22 - sig11) * .5 * sin2i + sig12 * cos2i);

        force += Vector<2>({bi, 0});
    }

    void Point::addForceContribution(const list<Point *> & points, Vector<2> & force,
                                     Vector<2> & v2, Vector<3> & stress) {
        for(const Point * p : points) {
            if(p == this) continue;
            addForceContribution(p, force, v2, stress);
        }
    }

    void Point::addForceContribution(const string & key, Vector<2> & force, Vector<2> & v2, Vector<3> & stress) {
        addForceContribution(getDomain()->getContainer(key), force, v2, stress);
    }
    
    /**
     * Add the cached force results.
     */
    void Point::sumCaches(Vector<2> & force, Vector<2> & forceGradient, Vector<3> & stress) const {
        for(unsigned i = 0; i < caches.size(); i++) {
            force += caches[i].getForce();
            forceGradient += caches[i].getForceGradient();
            stress += caches[i].getStress();
        }
    }
    
    Vector<2> Point::cachedForce() const {
        Vector<2> force;
        for(unsigned i = 0; i < caches.size(); i++) {
	    if (caches[i] == nullptr){
	      int j = i;
	    }
            force += caches[i].getForce();
        }
        return force;
    }
    Vector<2> Point::cachedGradient() const {
        Vector<2> gradient;
        for(unsigned i = 0; i < caches.size(); i++) {
            gradient += caches[i].getForceGradient();
        }
        return gradient;
    }
    Vector<3> Point::cachedStress() const {
        Vector<3> stress;
        for(unsigned i = 0; i < caches.size(); i++) {
            stress += caches[i].getStress();
        }
        return stress;
    }
    
    void Point::updateCaches() {
        for(unsigned i = 0; i < caches.size(); i++) {
            caches[i].update(this);
        }
    }
    
    void Point::updateLocation(PointLog projectedLocation) {
        if(getSlipPlane() != projectedLocation.slipPlane) {
            DdError::exception("Unimplemented.");
        }
        slipPlanePosition = projectedLocation.slipPlanePosition;
        history.push_front(new PointLog(projectedLocation));
    }
    void Point::updateLocation(const double & slipPlanePosition) {
        updateLocation(PointLog(getSlipPlane(), slipPlanePosition));
    }
}

