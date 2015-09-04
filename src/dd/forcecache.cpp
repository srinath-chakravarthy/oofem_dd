#include "forcecache.h"  
#include "vector.h"
#include "forcefunctor/forcefunctor.h"
#include "point.h"
    
namespace dd {

    ForceCache::ForceCache(ForceFunctor * functor) :
        functor(functor) { }
    ForceCache::~ForceCache() { }
    
    Vector<2> ForceCache::getForce() const { return force; }
    Vector<2> ForceCache::getForceGradient() const { return forceGradient; }
    Vector<3> ForceCache::getStress() const { return stress; }
        
    void ForceCache::update(Point * point) {
        force.zero();
        forceGradient.zero();
        stress.zero();
        functor->calculate(point, force, forceGradient, stress);
    }
}
