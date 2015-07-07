#ifndef HASHEDREGISTRABLE_H_INCLUDED
#define HASHEDREGISTRABLE_H_INCLUDED

#include "../../../sm/EngineeringModels/DD/ddobject.h"
#include <unordered_map>
#include <iostream>

namespace dd {

    template <typename T>
    class HashedRegistrable : public DdObject {
#define HASHEDREGISTRABLE_NAME "HashedRegistrable"
    public:
        std::unordered_map<string, list<T *>> containers;

        /**
         * Register element before the given iterator.
         */
        virtual typename list<T *>::iterator insert(T * toInsert,
                                                    typename list<T *>::iterator antecedent) {
            containers[toInsert->typeName()].insert(antecedent, toInsert);
            return --antecedent;
        }

        /**
         * Register the element at the end.
         */
        virtual typename list<T *>::iterator insert(T * toInsert) {
            return insert(toInsert, containers[toInsert->typeName()].end());
        }

        /**
         * Erase the element of the given iterator
         */
        virtual void erase(typename list<T *>::iterator toErase) {
            containers[(*toErase)->typeName()].erase(toErase);
        }

        /**
         * Get the container of the given type
         */
        template <typename U>
        list<T *> & getContainer() {
            return containers[U::staticTypeName()];
        }

        /**
         * Get the container of the given key
         */
        list <T *> & getContainer(const string & key) {
            return containers[key];
        }

        virtual string typeName() const { return HASHEDREGISTRABLE_NAME; }
        static string staticTypeName() { return HASHEDREGISTRABLE_NAME; }
    };
}

#endif // HASHEDREGISTRABLE_H_INCLUDED
