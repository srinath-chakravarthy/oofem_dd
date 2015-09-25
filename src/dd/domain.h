#pragma once
#include <vector>
#include "registrable/hashedregistrable.h"
#include "ddobject.h"
#include "vector.h"
#include "timemanager.h"
#include <string>
/** Definition of Domain for discrete dislocation dynamics in 2D
 * Also defines the basic namespace dd 
 * Class defines basic domain, with properties coming form OOFEM later
 * Conatins @param SlipSystem Information --> Container to all slipsystems
 * Conatins @param Point Information ---> Container to all points --> registered through a hash tabl
 */ 

namespace oofem { 
    class DDLinearStatic;
    class TimeStep;
}
    
namespace dd {

    class SlipSystem;
    class Point;
    class DislocationPoint;
    class DdCounter;
    class FemInterface;

    /**
     * Problem domain.
     */
    class Domain : public HashedRegistrable<Point> {
#define DOMAIN_NAME "Domain"
    private:
        double propModulus;                   /*!< Young's modulus ratio in this Domain */
        double propPassionsRatio;             /*!< Poisson's ratio in this Domain */ 
        std::vector<SlipSystem *> sSystems;   /*!< Container to Slipsystems for this domain */    
        FemInterface * femInterface;
    public:
	    /// Constructor to create domain Material Properties->modulus, poisson
	    Domain(const double & propModulus, const double & propPassionsRatio, FemInterface * femInterface = nullptr);
      
        virtual void updateForceCaches();
      
        /**
	     * Add the FEM force contribution at the given position.
	     * @param position The position to extract the force from
	     * @param force vector containing glide and climb forces 
	     * @param forceGradient vector contain gradient along slip direction and perpendicular to slip direction
	     * @param stress vector containing the 2D vector components of the stress (stress_xx, stress_yy, stress_xy)
	     */ 
        virtual void addFEMContribution(const Point * point, Vector<2> &force,
                                        Vector<2> &forceGradient, Vector<3> &stress) const;
      
      
        virtual void moveDislocations(double dt, double b);
      
        void addSlipSystem(SlipSystem * ss);
      	virtual void setFemInterface(FemInterface * femInterface);
      	virtual FemInterface * getFemInterface() const;

        double getModulus() const;
        double getPassionsRatio() const;

        virtual string typeName() const { return DOMAIN_NAME; }
        static string staticTypeName() { return DOMAIN_NAME; }
        
        /// Temporary hack for Linear Time dependent force and stress everywhere in the domain 
        int dtNo = 0; 
	int dtNomax = 100;
        
    };
}
