#ifndef COMPLEX_H
#define COMPLEX_H

#include "ddobject.h"
#include "vector.h"
#include <complex>

namespace dd {

    class Complex : public DdObject, public std::complex<double> {
#define COMPLEX_NAME "Complex"
    public:
        static const Complex i;

        Complex(const double & real, const double & imag) :
            std::complex<double>(real, imag) { }

        Complex() : Complex(0., 0.) { }

        Complex(const std::complex<double> & comp) :
            Complex(comp.real(), comp.imag()) { }

        Complex(const Vector<2> & vec);

        template <typename T>
        Complex operator*(const T & val) const {
            return static_cast<std::complex<double>>(*this) * val;
        }

        template <typename T>
        Complex operator+(const T & val) const {
            return static_cast<std::complex<double>>(*this) + val;
        }

        template <typename T>
        Complex operator-(const T & val) const {
            return (*this) + (-val);
        }

        template <typename T>
        Complex operator/(const T & val) const {
            return static_cast<std::complex<double>>(*this) / val;
        }

        Complex operator-() const;

        double abs() const;
        Complex conjugate() const;
        Complex log() const;
	Complex exp() const;
	

        virtual string typeName() const { return COMPLEX_NAME; }
        static string staticTypeName() { return COMPLEX_NAME; }
    };
}


#endif // COMPLEX_H
