#ifndef HASHEDREGISTRABLE_H_INCLUDED
#define HASHEDREGISTRABLE_H_INCLUDED

#include "registrable.h"
#include <unordered_map>

namespace dd {

	/**
     * A typename-hashed registrable for objects of type T. Container must be a container of
     * objects of type T *.
     */
    template <typename T, typename Container = list<T *>>
    class HashedRegistrable : public Registrable<T, Container> {
#define HASHEDREGISTRABLE_NAME "HashedRegistrable"
    public:
        std::unordered_map<string, Container> containers;

        /**
         * Register element before the given iterator.
         */
        virtual typename Container::iterator insert(T * toInsert,
                                                    typename Container::iterator antecedent) {
            containers[toInsert->typeName()].insert(antecedent, toInsert);
            return --antecedent;
        }

        /**
         * Register the element at the end.
         */
        virtual typename Container::iterator insert(T * toInsert) {
            return insert(toInsert, containers[toInsert->typeName()].end());
        }

        /**
         * Erase the element of the given iterator
         */
        virtual void erase(typename Container::iterator toErase) {
            containers[(*toErase)->typeName()].erase(toErase);
        }

        /**
         * Get the container of the given type
         */
        template <typename U>
        Container & getContainer() {
            return containers[U::staticTypeName()];
        }

        /**
         * Get the container of the given key
         */
        Container & getContainer(const string & key) {
            return containers[key];
        }

        virtual string typeName() const { return HASHEDREGISTRABLE_NAME; }
        static string staticTypeName() { return HASHEDREGISTRABLE_NAME; }
    };
}

#endif // HASHEDREGISTRABLE_H_INCLUDED
