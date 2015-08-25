#pragma once

#include "ddobject.h"
#include "domain.h"
#include "slipplane.h"
#include "registration.h"
#include "vector.h"
#include "forcecache.h"
#include <vector>
/** Abstract Point Class defining a basic Point 
 * Point is is registered to a class Domain and a SlipPlane
 * All other Points --> Dislocations, Sources, Obstacles inherit the characteristics of this class
 * All inherited points will be in containers (std::lists) and are hash mapped for ease of access
 */ 


namespace dd {

    struct PointLog {
        SlipPlane * slipPlane;
        double slipPlanePosition;
        PointLog(SlipPlane * slipPlane = nullptr, double slipPlanePosition = 0) :
            slipPlane(slipPlane), slipPlanePosition(slipPlanePosition) { }
    };

    /**
      * Abstract point class.
      */
    class Point : public DdObject {
#define POINT_NAME "Point"
    protected:
        std::list<PointLog *> history;
        std::vector<ForceCache> caches;
        Registration<Point, Domain> * domainRegistration = nullptr; /*!< Pointer to a registered Domain */
        Registration<Point, SlipPlane> * sPlaneRegistration = nullptr; /*!< pointer to a registerd SlipPlane */
        double slipPlanePosition = 0; /* !< Local Postion of point on SlipPlane */

        /**
         * Destruct domain registration
         */
        void destructDomainRegistration() {
            if(domainRegistration != nullptr) {
                delete domainRegistration;
                domainRegistration = nullptr;
            }
        }

        /**
         * Destruct sPlaneRegistration
         */
        void destructsPlaneRegistration() {
            if(sPlaneRegistration != nullptr) {
                delete sPlaneRegistration;
                sPlaneRegistration = nullptr;
            }
        }

        /**
         * Destruct both registrations
         */
        void destructRegistrations() {
            destructDomainRegistration();
            destructsPlaneRegistration();
        }

        /**
         * Sets the Registration objects.
         *
         * setRegistrations is implemented for registration after construction.
         * During the construction process, the correct type name string is not
         * produced due to unfinished vtable.
         */
        void setRegistrations(Domain * domain, SlipPlane * sPlane, const typename list<Point *>::iterator & antecedentIt) {
            destructRegistrations();
            if(domain != nullptr) {
                this->domainRegistration = new Registration<Point, Domain>(this,
                                                                           domain);
            }
            if(sPlane != nullptr) {
                this->sPlaneRegistration = new Registration<Point, SlipPlane>(this,
                                                                              sPlane,
                                                                              antecedentIt);
            }
        }

        /**
         * Externally set the registrations
         */
        void setRegistrations(Domain * domain, SlipPlane * sPlane) {
            destructRegistrations();
            if(domain != nullptr) {
                this->domainRegistration = new Registration<Point, Domain>(this,
                                                                           domain);
            }
            if(sPlane != nullptr) {
                this->sPlaneRegistration = new Registration<Point, SlipPlane>(this,
                                                                              sPlane);
            }
        }

    public:

        /**
         * Registers the point after the given iterator position within the sPlane.
         */
        Point(Domain * domain, SlipPlane * sPlane, typename list<Point *>::iterator antecedentIt,
              double slipPlanePosition) :
                  slipPlanePosition(slipPlanePosition) {
            setRegistrations(domain, sPlane, antecedentIt);
        }

        /**
         * Register the point to the end of the sPlane.
         */
        Point(Domain * domain, SlipPlane * sPlane, double slipPlanePosition) :
            slipPlanePosition(slipPlanePosition) {
            setRegistrations(domain, sPlane);
        }

        /**
         * Unregistered constructor
         */
        Point(double slipPlanePosition) :
            Point(nullptr, nullptr, slipPlanePosition) { }


        /**
         * Destructor
         */
        virtual ~Point() {
            destructRegistrations();
        }

        /// Returns the domain containing the Point
        Domain * getDomain() const { return domainRegistration->getTarget(); }
        /// Returns SlipPlane containing the point
        SlipPlane * getSlipPlane() const { return sPlaneRegistration->getTarget(); }
        /// Returns Position on the SlipPlane containing the point
        Vector<2> getLocation() const { return getSlipPlane()->getPointPosition(slipPlanePosition); }
        
        double getSlipPlanePosition() const { return slipPlanePosition; }


    /**
	 * Abstract method to get sign of burgers Vector at a point
	 * The inherited class will define the method
	 */ 
        virtual int getBurgersSign() const;
        virtual bool canSpawn() const = 0;
        virtual void spawn();

        double getBurgersMagnitude() const; /*!< Return Magnitude of BurgersVector from base slipsystem */
        Vector2d getBurgersVector() const; /* !< Return Burgers Vector from bas SlipSystem */ 

	/**
	 * Abstract method computing the Interaction force between 2 dislocations 
	 * @param Point Point contributing to the force
	 * @param force vector containing glide and climb forces 
	 * @param force_gradient vector contain gradient along slip direction and perpendicular to slip direction
	 * @param stress vector containing the 2D vector components of the stress (stress_xx, stress_yy, stress_xy)
	 */ 

        virtual void addForceContribution(const Point * &, Vector<2> &force,
                                          Vector<2> &force_gradient, Vector<3> &stress);

	/**
	 * Abstract method computing the Interaction force between 2 dislocations 
	 * @param Point_list List of Points contributing to the force, eg. all dislocations
	 * @param force vector containing glide and climb forces 
	 * @param force_gradient vector contain gradient along slip direction and perpendicular to slip direction
	 * @param stress vector containing the 2D vector components of the stress (stress_xx, stress_yy, stress_xy)
	 */ 
	
        virtual void addForceContribution(const list<Point *> &Point_list, Vector<2> &force,
                                          Vector<2> &force_gradient, Vector<3> &stress);
	
	/** Abstract method for computing interaction force between 2 dislocatios
	 * @param pointName string containing name of Point Container which contribute to force, eg. DislocationPoint, SourcePoint, ObstaclePoint
	 * @param force vector containing glide and climb forces 
	 * @param force_gradient vector contain gradient along slip direction and perpendicular to slip direction
	 * @param stress vector containing the 2D vector components of the stress (stress_xx, stress_yy, stress_xy)
	 */ 
	
        virtual void addForceContribution(const string &pointName, Vector<2> &force, 
					  Vector<2> &force_gradient, Vector<3> &stress);
	
		/**
	 * Abstract templated method computing the Interaction force between 2 dislocations 
	 * @param typename ---> DislocationPoint, SourcePoint, ObstaclePoint, VirtualDislocation 
	 * @param Point_list List of Points contributing to the force, eg. all dislocations
	 * @param force vector containing glide and climb forces 
	 * @param force_gradient vector contain gradient along slip direction and perpendicular to slip direction
	 * @param stress vector containing the 2D vector components of the stress (stress_xx, stress_yy, stress_xy)
	 */ 
        template <typename T>
        void addForceContribution(Vector<2> & force, Vector<2> & force_gradient, Vector<3> & stress) {
            addForceContribution(getDomain()->getContainer<T>(), force, force_gradient, stress);
        }
    
        /**
         * Add the cached force results.
         */
        void sumCaches(Vector<2> & force, Vector<2> & forceGradient, Vector<3> & stress);
    
        /**
         * Update the force caches.
         */
        virtual void updateCaches();
        
        /**
         * Update the location
         */
        virtual void updateLocation(PointLog projectedLocation);

        virtual string typeName() const { return POINT_NAME; }
        static string staticTypeName() { return POINT_NAME; }
    };
}
