#ifndef FORCECACHE_H
#define FORCECACHE_H

#include "ddobject.h"
#include "vector.h"
#include <memory>

namespace dd {

    class ForceFunctor;
    class Point;
   
    class ForceCache : public DdObject {
#define FORCECACHE_NAME "ForceCache"
    private:
        Vector<2> force;
        Vector<2> forceGradient;
        Vector<3> stress;
        ForceFunctor * __functor;
    public:
	/**
	 * Constructor.
	 * 
	 * @param functor   Force functor
	 */
        ForceCache(ForceFunctor * functor = nullptr);
        virtual ~ForceCache();
    
	/**
	 * Get the cached force.
	 * 
	 * @returns cached force
	 */
        Vector<2> getForce() const;
	
	/**
	 * Get the cached force gradient.
	 * 
	 * @returns cached force gradient.
	 */
        Vector<2> getForceGradient() const;
        
	/**
	 * Get the cached stress
	 * 
	 * @returns cached stress.
	 */
	Vector<3> getStress() const;
        
	/**
	 * Update the cache
	 * 
	 * @param point	   Point for which the force is calculated
	 */
        void update(Point * point);
        
        virtual string typeName() const { return FORCECACHE_NAME; }
        static string staticTypeName() { return FORCECACHE_NAME; }
    };

}

#endif
