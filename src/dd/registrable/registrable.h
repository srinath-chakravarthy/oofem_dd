#ifndef REGISTRABLE_H
#define REGISTRABLE_H

#include "ddobject.h"
#include <list>

namespace dd {

    /**
     * Abstract template for registrables of items of T,
     * into Containers of T pointers.
     */
    template<typename T, typename Container = std::list<T *>>
    class Registrable : public DdObject {
#define REGISTRABLE_NAME "HashedRegistrable"
    public:
        /**
         * Insert the given entity after the given iterator
         * 
         * @param toInsert         The entity to be inserted
         * @param antecedent       The iterator after which the item is to be inserted
         * @returns                Iterator to the inserted entity
         */
        virtual typename Container::iterator insert(T * toInsert,
                                                    typename Container::iterator antecedent) = 0;
                                                   
        /**
         * Insert the given entity at the end of the pertinent container
         * 
         * @param toInsert         The entity to be inserted
         * @returns                Iterator to the inserted entity
         */
        virtual typename Container::iterator insert(T * toInsert) = 0;
        
        /**
         * Erase the element of the given iterator
         * 
         * @param toErase          The given iterator to be erased                  
         */
        virtual void erase(typename Container::iterator toErase) = 0;
        
        
        virtual string typeName() const { return REGISTRABLE_NAME; }
        static string staticTypeName() { return REGISTRABLE_NAME; }
    };
}


#endif