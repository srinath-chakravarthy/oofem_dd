#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>
#include <cmath>
#include "../oofemlib/floatarray.h"

namespace dd {


    /**
     * Fixed-size vector class.
     *
     * TODO: Write allocation-free assignment operators;
     * work infix operators through assignment operators.
     */
    template <unsigned size>
    class Vector {
#define VECTOR_NAME "Vector"
    private:
        double data[size];
    public:

        /**
         * Size getter
         */
        constexpr double getSize() const { return size; }

        /**
         * Immutable get
         */
        const double & operator[](unsigned index) const {
            return data[index];
        }

        /**
         * Mutable get
         */
        double & operator[](unsigned index) {
            return data[index];
        }

        /**
         * Populate the vector with the given value.
         */
        void fill(const double & value) {
           for(unsigned i = 0; i < size; i++) {
                data[i] = value;
            }
        }
        
        double abs() const {
            double sum = 0;
            for(int i = 0; i < getSize(); i++) {
                sum += data[i] * data[i];
            }
            return ::sqrt(sum);
        }

        /**
         * Zero-fill
         */
        void zero() { fill(0); }

        /**
         * Fill constructor
         */
        Vector(const double & value) { fill(value); }

        /**
         * Scale the vector
         */
        void scale(double scaleValue) {
            for(unsigned i = 0; i < size; i++) {
                data[i] *= scaleValue;
            }
        }

        /**
         * Zero constructor
         */
        Vector() : Vector(0) { }

        /**
         * Copy constructor
         */
        Vector(const Vector & other) {
            for(unsigned i = 0; i < size; i++) {
                data[i] = other[i];
            }
        }

        /**
         * Initializer-list constructor
         */
        Vector(const std::initializer_list<double> & init) {
            unsigned index = 0;
            for(double val : init) {
                data[index] = val;
                index++;
            }
        }

        /**
         * Unary minus
         */
        Vector operator-() const {
            return Vector(*this).scale(-1);
        }

        /**
         * Vector addition
         */
        Vector operator+(const Vector & other) const {
            Vector result;
            for(unsigned i = 0; i < size; i++) {
                result[i] = data[i] + other[i];
            }
            return result;
        }
        Vector & operator+=(const Vector & other) {
            return *this = (*this + other);
        }

        /**
         * Vector subtraction
         */
        Vector operator-(const Vector & other) const {
            return (*this) + (-Vector(other));
        }
        Vector & operator-=(const Vector & other) {
            return *this = (*this - other);
        }

        /**
         * Scaling
         */
        Vector & operator*=(const double & factor) {
            scale(factor);
            return *this;
        }
        Vector operator*(const double & factor) const {
            return Vector(*this) *= factor;
        }
        /**
         * Division
         */
        Vector & operator/=(const double & factor) {
            for(unsigned i = 0; i < size; i++) {
                data[i] /= factor;
            }
        }
        Vector operator/(const double & factor) { 
            return Vector(*this) /= factor;
        }

        virtual string typeName() const { return VECTOR_NAME; }
        static string staticTypeName() { return VECTOR_NAME; }
        
        oofem::FloatArray toArray() const {
            oofem::FloatArray result;
            result.resize(size);
            for(long long i = 0; i < size; i++) {
                result[i] = data[i];
            }
            return result;
        }
        
        operator oofem::FloatArray() const {
            return toArray();
        }
    };

    typedef Vector<2> Vector2d;
    typedef Vector<3> Vector3d;
}

#endif // VECTOR_H
