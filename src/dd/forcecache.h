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
        std::shared_ptr<ForceFunctor> functor;
    public:
        ForceCache(ForceFunctor * functor = nullptr);
        virtual ~ForceCache();
    
    
        Vector<2> getForce() const;
        Vector<2> getForceGradient() const;
        Vector<3> getStress() const;
        
        void update(Point * point);
        
        virtual string typeName() const { return FORCECACHE_NAME; }
        static string staticTypeName() { return FORCECACHE_NAME; }
    };

}

#endif
