#include "complex.h"
#include <cmath>
#include <complex>
#include "vector.h"
#include <iostream>
namespace dd {

    const Complex Complex::i = Complex(0, 1);
    Complex::Complex(const Vector<2> & vec) :
        Complex::Complex(vec[0], vec[1]) { }

    Complex Complex::operator-() const {
        return Complex(-real(), -imag());
    }

    double Complex::abs() const { return std::abs(*this); }
    Complex Complex::conjugate() const { return Complex(real(), -imag()); }
    Complex Complex::log() const { return std::log(*this); }
    Complex Complex:: exp() const {return std::exp(*this);}
}
