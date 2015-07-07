#ifndef VECTOR_H
#define VECTOR_H

#include <initializer_list>

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
        Vector operator*(const double & factor) const {
            Vector result(*this);
            result.scale(-1);
            return result;
        }
        Vector & operator*=(const double & factor) {
            return *this = (*this * factor);
        }

        virtual string typeName() const { return VECTOR_NAME; }
        static string staticTypeName() { return VECTOR_NAME; }
    };

    typedef Vector<2> Vector2d;
    typedef Vector<3> Vector3d;
}

#endif // VECTOR_H
