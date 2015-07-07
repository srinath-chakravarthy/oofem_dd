#pragma once
#include <vector>
#include "../../../sm/EngineeringModels/DD/hashedregistrable.h"
#include "../../../sm/EngineeringModels/DD/ddobject.h"
#include "../../../sm/EngineeringModels/DD/vector.h"
#include <string>
/** Definition of Domain for discrete dislocation dynamics in 2D
 * Also defines the basic namespace dd 
 * Class defines basic domain, with properties coming form OOFEM later
 * Conatins @param SlipSystem Information --> Container to all slipsystems
 * Conatins @param Point Information ---> Container to all points --> registered through a hash tabl
 */ 

namespace dd {

    class SlipSystem;
    class Point;
    class DislocationPoint;

    /**
     * Problem domain.
     */
    class Domain : public HashedRegistrable<Point> {
#define DOMAIN_NAME "Domain"
    private:
        double propModulus;                   /*!< Young's modulus ratio in this Domain */
        double propPassionsRatio;             /*!< Poisson's ratio in this Domain */ 
        std::vector<SlipSystem *> sSystems;   /*!< Container to Slipsystems for this domain */       
    public:
	/// Constructor to create domain Material Properties->modulus, poisson
	Domain(const double & propModulus, const double & propPassionsRatio);
      	/// Constructor to create domain with SlipSystemCount, Material Properties->modulus, poisson
        Domain(long long slipSystemCount, const double & propModulus, const double & propPassionsRatio);
        Domain();


        void addSlipSystem(SlipSystem * ss);


        double getModulus() const;
        double getPassionsRatio() const;

        virtual string typeName() const { return DOMAIN_NAME; }
        static string staticTypeName() { return DOMAIN_NAME; }
    };
}
