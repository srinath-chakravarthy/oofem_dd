#include "point.h"
#include "dderror.h"
#include "vector.h"
#include "domain.h"
#include "complex.h"
#include "forcefunctor/fromfem.h"
namespace dd {
    void Point::move() {
        DdError::exception("Point cannot move.");
    }

    void Point::spawn() {
        DdError::exception("Point cannot spawn.");
    }

    void Point::remove() {
        DdError::exception("Point cannot be removed.");
    }

    double Point::getBurgersMagnitude() const {
        return getSlipPlane()->getBurgersMagnitude();
    }

    int Point::getBurgersSign() const { return 0; }

    Vector<2> Point::getBurgersVector() const {
        return getSlipPlane()->getBurgersVector();
    }

    void Point::addForceContribution(const Point * & p, Vector<2> & force,
                                     Vector<2> & v2, Vector<3> & stress) {
        double thetaz = getDomain()->getModulus();
        double factor = getDomain()->getPassionsRatio();

        double cutOff = 2 * std::abs(this->getBurgersMagnitude());
        Complex b(this->getBurgersVector());
        Complex z(this->getLocation());

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
    
    void Point::updateCaches() {
        for(unsigned i = 0; i < caches.size(); i++) {
            caches[i].update();
        }
    }
    
    void Point::updateLocation() {
        if(getSlipPlane() != projectedLocation.slipPlane) {
            throw(3); // TODO
        }
        slipPlanePosition = projectedLocation.slipPlanePosition;
    }
}

