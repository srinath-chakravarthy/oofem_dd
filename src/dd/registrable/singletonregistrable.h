#ifndef SINGLETONREGISTRABLE_H
#define SINGLETONREGISTRABLE_H

#include "registrable/registrable.h"

namespace dd {
 
    /**
     * Single-container registrable for items of type T
     * 
     */
    template<typename T, typename Container = std::list<T *>>
    class SingletonRegistrable : public Registrable<T, Container> {
#define SINGLETONREGISTRABLE_NAME "SingletonRegistrable"
    private:
        Container __container;
    public:
        SingletonRegistrable() { }
        virtual ~SingletonRegistrable() { }
        
        virtual typename Container::iterator insert(T * toInsert,
                                                    typename Container::iterator antecedent) {
            __container.insert(antecedent, toInsert);
            return --antecedent;
        }
        
        virtual typename Container::iterator insert(T * toInsert) {
            return insert(toInsert, __container.end());
        }
        
        virtual void erase(typename Container::iterator toErase) {
            __container.erase(toErase);
        }
        
        /**
         * Get the internal container
         * 
         * @returns the internal container
         */
        virtual Container & container() {
            return __container;
        }
        
        virtual string typeName() const { return SINGLETONREGISTRABLE_NAME; }
        static string staticTypeName() { return SINGLETONREGISTRABLE_NAME; }
    };
    
}

#endif