#ifndef EXPLICITPOINTREGISTRABLE_H
#define EXPLICITPOINTREGISTRABLE_H

#include "registrable"
#include "point/dislocation.h"
#include "point/obstacle.h"
#include "point/source.h"
#include "point/virtualdislocation.h"
#include <list>

namespace dd {
    
    /**
     * Hardcoded registrable of Containers of predefined point types.
     */
    class ExplicitPointRegistrable : public Registrable<Point, std::list<Point *>> {
#define EXPLICITPOINTREGISTRABLE_NAME "ExplicitPointRegistrable"
    private:
        std::list<DislocationPoint *> __dislocations;
        std::list<SourcePoint *> __sources;
        std::list<ObstaclePoint *> __obstacles;
        std::list<VirtualDislocationPoint *> __virtualDislocations;
    public:
        
        
        
        /**
         * Insert the given entity after the given iterator
         * 
         * @param toInsert         The entity to be inserted
         * @param antecedent       The iterator after which the item is to be inserted
         * @returns                Iterator to the inserted entity
         */
        virtual typename Container::iterator insert(T * toInsert,
                                                    typename Container::iterator antecedent);
        
        /**
         * Insert the given entity at the end of the pertinent container
         * 
         * @param toInsert         The entity to be inserted
         * @returns                Iterator to the inserted entity
         */
        virtual typename Container::iterator insert(T * toInsert);
        
        /**
         * Erase the element of the given iterator
         * 
         * @param toErase          The given iterator to be erased                  
         */
        virtual void erase(typename Container::iterator toErase);
        
        virtual string typeName() const { return EXPLICITPOINTREGISTRABLE_NAME; }
        static string staticTypeName() { return EXPLICITPOINTREGISTRABLE_NAME; }
    };
    
    
}

#endif