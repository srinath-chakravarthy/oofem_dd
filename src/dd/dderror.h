#ifndef DDERROR_H
#define DDERROR_H

#include <stdexcept>
#include <string>

namespace dd {
    class DdError : public DdObject {
#define DDERROR_NAME "DdError"
    private:
        std::string message;
    public:
        DdError(std::string message) : message(message) { }
        void throwSelf() const {
            throw std::runtime_error(message);
        }

        static void exception(std::string str) {
            throw std::runtime_error(str);
        }

        virtual string typeName() const { return DDERROR_NAME; }
        static string staticTypeName() { return DDERROR_NAME; }
    };
}

#endif // DDERROR_H
