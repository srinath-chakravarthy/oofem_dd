#include "forcecache.h"  
#include "vector.h"
#include "forcefunctor/forcefunctor.h"
#include "point.h"
    
namespace dd {

    ForceCache::ForceCache(ForceFunctor * functor) :
        __functor(functor) { }
    ForceCache::~ForceCache() { }
    
    Vector<2> ForceCache::getForce() const { return force; }
    Vector<2> ForceCache::getForceGradient() const { return forceGradient; }
    Vector<3> ForceCache::getStress() const { return stress; }
        
    void ForceCache::update(Point * point) {
        force.zero();
        forceGradient.zero();
        stress.zero();
        __functor->calculate(point, force, forceGradient, stress);
    }
}
