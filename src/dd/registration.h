#ifndef REGISTRATION_H_INCLUDED
#define REGISTRATION_H_INCLUDED

#include "registrable/hashedregistrable.h"

namespace dd {

    /**
      * Registration of object type T to the container of type Registrable
      *
      */
    template <typename T, typename RegistrableContainer = list<T *>, typename Registrable = HashedRegistrable<T, RegistrableContainer>>
    class Registration {
#define REGISTRATION_NAME "Registration"
    private:
        Registrable * __target;
        typename RegistrableContainer::iterator registreeIterator;
    public:
    
    	/**
    	 * Registration target getter
    	 *
    	 * @returns registration target
    	 */
        Registrable * target() const { return __target; }
        
        /**
         * Registration iterator getter
         *
         * @returns registration iterator
         */
        typename RegistrableContainer::iterator getIterator() const { return registreeIterator; }
        
        /**
	 * Default constructor
	 *
	 * @param toRegister 	object to be registered
	 * @param target	target registration
	 */
        Registration(T * toRegister, Registrable * target) {
            this->__target = target;
            registreeIterator = target->insert(toRegister);
        }
        
        Registration(T * toRegister, Registrable * target, typename RegistrableContainer::iterator antecedent) {
            this->__target = target;
            registreeIterator = target->insert(toRegister, antecedent);
        }
        
        
        virtual ~Registration() {
            __target->erase(registreeIterator);
        }


        virtual string typeName() const { return REGISTRATION_NAME; }
        static string staticTypeName() { return REGISTRATION_NAME; }
    };

}


#endif // REGISTRATION_H_INCLUDED
