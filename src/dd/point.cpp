#include "point.h"
#include "dderror.h"
#include "vector.h"
#include "domain.h"
#include "complex.h"
#include "forcefunctor/fromfem.h"
#include "forcefunctor/fromfemmock.h"
#include "dderror.h"
#include <cmath>
#include <iostream>
namespace dd {

    void Point::destructDomainRegistration() { 
	if(domainRegistration != nullptr) {
	    delete domainRegistration;
	    domainRegistration = nullptr;
	}
    }
            
    void Point::destructPlaneRegistration() {
	if(sPlaneRegistration != nullptr) {
	    delete sPlaneRegistration;
	    sPlaneRegistration = nullptr;
	}
    }

    void Point::destructRegistrations() {
      destructPlaneRegistration();
      destructDomainRegistration();
    }
    
    void Point::setRegistrations(Domain * domain, SlipPlane * sPlane, const typename list<Point *>::iterator & antecedentIt) {
	destructRegistrations();
	if(domain != nullptr) {
	    this->domainRegistration = new Registration<Point, list< Point *>, Domain>(this,
													domain);
	}
	if(sPlane != nullptr) {
	    this->sPlaneRegistration = new Registration<Point, list< Point *>, SlipPlane>(this,
												      sPlane,
												      antecedentIt);
	}
    }
    
    void Point::setRegistrations(Domain * domain, SlipPlane * sPlane) {
	destructRegistrations();
	if(domain != nullptr) {
	    this->domainRegistration = new Registration<Point, list< Point *>, Domain>(this,
									domain);
	}
	if(sPlane != nullptr) {
	    this->sPlaneRegistration = new Registration<Point, list< Point *>, SlipPlane>(this,
									  sPlane);
	}
    }
    
    double Point::getBurgersMagnitude() const { return slipPlane()->getBurgersMagnitude(); }
    int Point::getBurgersSign() const { return 1; }
    Vector<2> Point::getBurgersVector() const { return slipPlane()->getBurgersVector(); }


    
    void Point::addForceContribution(const Point * & p, Vector<2> & force,
                                 Vector<2> & v2, Vector<3> & stress) {
    if (p != this) {
        double e = domain()->getModulus();
        double nu = domain()->getPassionsRatio();
        double mu = e / (2. * (1. + nu));
        double factor = mu / (4. * M_PI * (1. - nu));

        double cutOff = 2 * std::abs(this->getBurgersMagnitude());

        double sig11 = 0;
        double sig22 = 0;
        double sig12 = 0;
        double sig11z = 0;
        double sig22z = 0;
        double sig12z = 0;
	
        Complex b(this->getBurgersVector());   /// BurgerVector of evaluation point
        Complex z(this->location());           /// Location of evaluation point
        Complex thetaz(this->slipPlane()->getCos(), -this->slipPlane()->getSin()); /// Angles of evaluation point

	
	Complex zSource(p->location());       /// Point from which evaluation happens
	Complex bSource; 
	bSource = Complex(p->getBurgersVector() * p->getBurgersSign()); /// BurgerVector from which evaluation happens
        Complex dz = z - zSource;
        double zd = dz.abs();


        if(zd > cutOff) {
            Complex phi1 = -1. * Complex::i * factor * bSource / dz;
            Complex phi11 = Complex::i * factor * bSource / (dz * dz);
            Complex phi2 = Complex::i * factor * bSource.conjugate() / dz;

            Complex tmp1 = 2. * Complex(phi1 +  phi1.conjugate());
            Complex tmp2 = -2. * Complex(dz * phi11.conjugate() + phi2.conjugate());

            sig11 += Complex(0.5 * (tmp1 + tmp2)).real();
            sig22 += Complex(0.5 * (tmp1 - tmp2)).real();
            sig12 += Complex(.5 * tmp2).imag();

            Complex phi1z = Complex::i * factor * bSource / (dz * dz) * thetaz;
            Complex phi11z = -2. * Complex::i * factor * bSource / (dz * dz * dz) * thetaz;
            Complex phi2z = -1. * factor * bSource.conjugate() / (dz * dz) * thetaz;
            Complex tmp1z = 2. * (phi1z + phi1z.conjugate());
            Complex tmp2z = -2. * (dz * phi11z.conjugate() + phi2z.conjugate());

            sig11z += Complex(.5 * (tmp1z + tmp2z)).real();
            sig22z += Complex(.5 * (tmp1z - tmp2z)).real();
            sig12z += Complex(.5 * tmp2z).imag();
        }
        else {
            Complex th;
            if (zd > 1.e-4) {
                if (dz.real() == 0.) {
                    th = Complex(0., ::asin(this->slipPlane()->getSin()));
                }
                else {
                    th = Complex(0., dz.log().imag());
                }
                Complex zstar = cutOff * th.exp();
                Complex phi1 = -1. * Complex::i * factor * bSource / zstar;
                Complex phi11 = 1. *Complex::i * factor * bSource / (zstar*zstar);
                Complex phi2 = 1. * Complex::i * factor * bSource.conjugate() / zstar;

                Complex tmp1 = 2. * Complex(phi1 * phi1.conjugate());
                Complex tmp2 = -2. * Complex(zstar * phi11.conjugate() * phi2.conjugate());

                sig11 += Complex(0.5 * (tmp1 + tmp2)).real();
                sig22 += Complex(0.5 * (tmp1 - tmp2)).real();
                sig12 += Complex(.5 * tmp2).imag();

            }
        }

        double cos2i = ::cos(2. * this->slipPlane()->getAngle());
        double sin2i = ::sin(2. * this->slipPlane()->getAngle());

        double fg = slipPlane()->getBurgersMagnitude() * getBurgersSign() *
                    ((sig22 - sig11) * .5 * sin2i + sig12 * cos2i);

        double fgg = slipPlane()->getBurgersMagnitude() * getBurgersSign() *
                     ((sig22z - sig11z) * .5 * sin2i + sig12z * cos2i);

        force += Vector<2>( {fg, 0.0});
        stress += Vector<3>( {sig11, sig22, sig12});
        v2 += Vector<2>( {fgg,0.0});
    }
}

    void Point::addForceContribution(const list<Point *> & points, Vector<2> & force,
                                     Vector<2> & v2, Vector<3> & stress) {
        for(const Point * p : points) {
            if(p == this) continue;
            addForceContribution(p, force, v2, stress);
        }
    }

    void Point::addForceContribution(const string & key, Vector<2> & force, Vector<2> & v2, Vector<3> & stress) {
        addForceContribution(domain()->getContainer(key), force, v2, stress);
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
        if(slipPlane() != projectedLocation.slipPlane) {
            DdError::exception("Unimplemented.");
        }
        __slipPlanePosition = projectedLocation.slipPlanePosition;
        history.push_front(new PointLog(projectedLocation));
    }
    void Point::updateLocation(const double & slipPlanePosition) {
        updateLocation(PointLog(slipPlane(), slipPlanePosition));
    }
   
    void Point::destroy() {
        delete this;
    }
}

