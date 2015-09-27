#ifndef EXPLICITPOINTREGISTRABLE_H
#define EXPLICITPOINTREGISTRABLE_H

#include "registrable/registrable.h"
#include "point/dislocation.h"
#include "point/obstacle.h"
#include "point/source.h"
#include "point/virtualdislocation.h"
#include <list>

namespace dd {
    
    /**
     * Hardcoded registrable of Containers of predefined point types.
     */
    template<typename Container = std::list<Point *>>
    class ExplicitPointRegistrable : public Registrable<Point, std::list<Point *>> {
#define EXPLICITPOINTREGISTRABLE_NAME "ExplicitPointRegistrable"
    private:
        Container __dislocations;
        Container __sources;
        Container __obstacles;
        Container __virtualDislocations;
    public:
        
        /**
         * Virtual destructor
         */
        virtual ~ExplicitPointRegistralbe() { }
        
        /**
         * Get the container of the given type
         * 
         * @param T             The type to be checked
         * returns              Pointer to the container
         */
        template<typename T>
        virtual Container * container();
        
        /**
         * Get the container of the given point
         * 
         * @param point         The point whose type is to be checked
         * @returns             Pointer to the container
         * /
        
        virtual typename Container::iterator insert(Point * toInsert,
                                                    typename Container::iterator antecedent);
        virtual typename Container::iterator insert(Point * toInsert);
        virtual void erase(typename Container::iterator toErase);
        
        virtual string typeName() const { return EXPLICITPOINTREGISTRABLE_NAME; }
        static string staticTypeName() { return EXPLICITPOINTREGISTRABLE_NAME; }
    };
}

#endif