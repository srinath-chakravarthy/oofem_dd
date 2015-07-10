#ifndef BETWEENPOINTS_H_INCLUDED
#define BETWEENPOINTS_H_INCLUDED

#include <list>

namespace dd {

    /**
     * Entity located between two points of class T.
     */
    template <typename T>
    class BetweenPoints {
#define BETWEENPOINTS_NAME "BetweenPoints"
    private:
        typename list<T *>::iterator next;
        typename list<T *>::reverse_iterator prev;
    public:
        BetweenPoints(typename list<T *>::iterator next, typename list<T *>::reverse_iterator prev) :
            next(next), prev(prev) { }

        T * getNext() const { return next; }
        T * getPrev() const { return prev; }

        void forward() {
            next++;
            prev--;
        }

        void backward() {
            next--;
            prev++;
        }

        virtual string typeName() const { return BETWEENPOINTS_NAME; }
        static string staticTypeName() { return BETWEENPOINTS_NAME; }
    };

}

#endif // BETWEENPOINTS_H_INCLUDED
