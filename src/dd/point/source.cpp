#include "point/source.h"
#include "../forcefunctor/fromdislocations.h"
#include "../forcefunctor/fromfem.h"
#include <string>

namespace dd {


    void SourcePoint::setCaches() {
        caches.clear();
        caches.reserve(2);
        caches.push_back(ForceCache(FromDislocations::getInstance()));
        caches.push_back(ForceCache(FromFem::getInstance()));
    }
    
    
}
