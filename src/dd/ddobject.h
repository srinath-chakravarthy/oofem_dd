#pragma once
#include <string>
#include <sstream>
#include <typeinfo>
#include <list>

namespace dd {

    typedef std::string string;
    typedef std::stringstream stringstream;
    template <typename T>
    using list = std::list<T>;

    /**
      * Base Java object for DD.
      */
    class DdObject {
#define DDOBJECT_NAME "DdObject"
    public:
        /**
          * Dump to string.
          */
        virtual string toString() const {
            stringstream ss;
            ss << (void*) this;
            return ss.str();
        }

        /**
          * Extensional equality
          */
        virtual bool equals(const DdObject * other) const {
            return this == other;
        }

        /**
          * Intensional equality
          */
        bool operator==(const DdObject * other) const {
            return equals(other);
        }

        virtual string typeName() const { return DDOBJECT_NAME; }
        static string staticTypeName() { return DDOBJECT_NAME; }

   };
}
