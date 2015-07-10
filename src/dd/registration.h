#ifndef REGISTRATION_H_INCLUDED
#define REGISTRATION_H_INCLUDED

#include "hashedregistrable.h"

namespace dd {

    template <typename T, typename Registrable = HashedRegistrable<T>>
    class Registration {
#define REGISTRATION_NAME "Registration"
    private:
        Registrable * target;
        typename list<T *>::iterator registreeIterator;
    public:
        Registrable * getTarget() const { return target; }
        typename list<T *>::iterator getIterator() const { return registreeIterator; }
        Registration(T * toRegister, Registrable * target) {
            this->target = target;
            registreeIterator = target->insert(toRegister);
        }
        Registration(T * toRegister, Registrable * target, typename list<T *>::iterator antecedent) {
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
