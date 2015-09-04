#ifndef REGISTRATION_H_INCLUDED
#define REGISTRATION_H_INCLUDED

#include "hashedregistrable.h"

namespace dd {

	/**
	 * Registration of object type T to the container of type Registrable
	 *
	 */
    template <typename T, typename Registrable = HashedRegistrable<T>>
    class Registration {
#define REGISTRATION_NAME "Registration"
    private:
        Registrable * target;
        typename list<T *>::iterator registreeIterator;
    public:
    
    	/**
    	 * Registration target getter
    	 *
    	 * @returns registration target
    	 */
        Registrable * getTarget() const { return target; }
        
        /**
         * Registration iterator getter
         *
         * @returns registration iterator
         */
        decltype(registreeIterator) getIterator() const { return registreeIterator; }
        
        /**
 		 * Default constructor
 		 *
 		 * @param toRegister 	object to be registered
 		 * @param target		target registration
 		 */
        Registration(T * toRegister, Registrable * target) {
            this->target = target;
            registreeIterator = target->insert(toRegister);
        }
        
        Registration(T * toRegister, Registrable * target, decltype(registreeIterator) antecedent) {
            this->target = target;
            registreeIterator = target->insert(toRegister, antecedent);
        }
        
        
        virtual ~Registration() {
            target->erase(registreeIterator);
        }


        virtual string typeName() const { return REGISTRATION_NAME; }
        static string staticTypeName() { return REGISTRATION_NAME; }
    };

}


#endif // REGISTRATION_H_INCLUDED
