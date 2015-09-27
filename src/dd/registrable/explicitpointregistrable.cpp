#include "registrable/explicitpointregistrable.h"

namespace dd {
       
    template<typename Container>        
    typename Container::iterator ExplicitPointRegistrable::insert(Point * toInsert,
                                                    typename Container::iterator antecedent) {
    
    }
    typename Container::iterator insert(Point * toInsert);
    void erase(typename Container::iterator toErase);
    
}
