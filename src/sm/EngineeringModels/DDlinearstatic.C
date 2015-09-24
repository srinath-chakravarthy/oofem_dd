/*
 *
 *                 #####    #####   ######  ######  ###   ###
 *               ##   ##  ##   ##  ##      ##      ## ### ##
 *              ##   ##  ##   ##  ####    ####    ##  #  ##
 *             ##   ##  ##   ##  ##      ##      ##     ##
 *            ##   ##  ##   ##  ##      ##      ##     ##
 *            #####    #####   ##      ######  ##     ##
 *
 *
 *             OOFEM : Object Oriented Finite Element Code
 *
 *               Copyright (C) 1993 - 2013   Borek Patzak
 *
 *
 *
 *       Czech Technical University, Faculty of Civil Engineering,
 *   Department of Structural Mechanics, 166 29 Prague, Czech Republic
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "DDlinearstatic.h"
#include "../sm/Elements/structuralelement.h"
#include "../sm/Elements/structuralelementevaluator.h"
#include "nummet.h"
#include "timestep.h"
#include "element.h"
#include "dof.h"
#include "sparsemtrx.h"
#include "verbose.h"
#include "classfactory.h"
#include "datastream.h"
#include "contextioerr.h"
#include "classfactory.h"
#include "unknownnumberingscheme.h"
#include "spatiallocalizer.h"
#include "generalboundarycondition.h"
#include "node.h"
#include "manualboundarycondition.h"

#ifdef __PARALLEL_MODE
#include "problemcomm.h"
#include "communicator.h"
#include <material.h>
#endif

#include <typeinfo>

/* All the DD includes */
#include "../dd/ddobject.h"
#include "../dd/domain.h"
#include "../dd/point.h"
#include "../dd/dderror.h"
#include "../dd/slipsystem.h"
#include "../dd/slipplane.h"
#include <iostream>
#include "../dd/point/dislocation.h"
#include "../dd/point/obstacle.h"
#include "../dd/point/source.h"
#include <list>
#include "../dd/complex.h"
#include "../dd/vector.h"
#include "../dd/feminterface/oofeminterface.h"
#define _USE_MATH_DEFINES
#include <cmath>

#include "../dd/forcefunctor/fromfem.h"
#include "../dd/forcefunctor/fromfemmock.h"
#include "../dd/forcefunctor/fromdislocations.h"


namespace oofem {
REGISTER_EngngModel(DDLinearStatic);

DDLinearStatic :: DDLinearStatic(int i, EngngModel *_master) : StructuralEngngModel(i, _master), loadVector(), displacementVector()
{
    ndomains = 1;
    initFlag = 1;
    solverType = ST_Direct;
}


DDLinearStatic :: ~DDLinearStatic() { }


NumericalMethod *DDLinearStatic :: giveNumericalMethod(MetaStep *mStep)
{
    if ( !nMethod ) {
        if ( isParallel() ) {
            if ( ( solverType == ST_Petsc ) || ( solverType == ST_Feti ) ) {
                nMethod.reset( classFactory.createSparseLinSolver(solverType, this->giveDomain(1), this) );
            }
        } else {
            nMethod.reset( classFactory.createSparseLinSolver(solverType, this->giveDomain(1), this) );
        }
        if ( !nMethod ) {
            OOFEM_ERROR("linear solver creation failed");
        }
    }


    return nMethod.get();
}

IRResultType
DDLinearStatic :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                // Required by IR_GIVE_FIELD macro

    result = StructuralEngngModel :: initializeFrom(ir);
    if ( result != IRRT_OK ) {
        return result;
    }

    int val = 0;
    IR_GIVE_OPTIONAL_FIELD(ir, val, _IFT_EngngModel_lstype);
    solverType = ( LinSystSolverType ) val;

    val = 0;
    IR_GIVE_OPTIONAL_FIELD(ir, val, _IFT_EngngModel_smtype);
    sparseMtrxType = ( SparseMtrxType ) val;

#ifdef __PARALLEL_MODE
    if ( isParallel() ) {
        commBuff = new CommunicatorBuff( this->giveNumberOfProcesses() );
        communicator = new NodeCommunicator(this, commBuff, this->giveRank(),
                                            this->giveNumberOfProcesses());
    }

#endif


    return IRRT_OK;
}


double DDLinearStatic :: giveUnknownComponent(ValueModeType mode, TimeStep *tStep, Domain *d, Dof *dof)
// returns unknown quantity like displacement, velocity of equation eq
// This function translates this request to numerical method language
{
    int eq = dof->__giveEquationNumber();
#ifdef DEBUG
    if ( eq == 0 ) {
        OOFEM_ERROR("invalid equation number");
    }
#endif

    if ( tStep != this->giveCurrentStep() ) {
        OOFEM_ERROR("unknown time step encountered");
        return 0.;
    }

    switch ( mode ) {
    case VM_Total:
    case VM_Incremental:
        if ( displacementVector.isNotEmpty() ) {
            return displacementVector.at(eq);
        } else {
            return 0.;
        }

    default:
        OOFEM_ERROR("Unknown is of undefined type for this problem");
    }

    return 0.;
}


TimeStep *DDLinearStatic :: giveNextStep()
{
    if ( !currentStep ) {
        // first step -> generate initial step
        //currentStep.reset( new TimeStep(*giveSolutionStepWhenIcApply()) );
        currentStep.reset( new TimeStep(giveNumberOfTimeStepWhenIcApply(), this, 1, 0., 1., 0) );
    }
    previousStep = std :: move(currentStep);
    currentStep.reset(new TimeStep(*previousStep, 1.) );

    return currentStep.get();
}


void DDLinearStatic :: solveYourself()
{
    if ( this->isParallel() ) {
#ifdef __VERBOSE_PARALLEL
        // force equation numbering before setting up comm maps
        int neq = this->giveNumberOfDomainEquations(1, EModelDefaultEquationNumbering());
        OOFEM_LOG_INFO("[process rank %d] neq is %d\n", this->giveRank(), neq);
#endif

        this->initializeCommMaps();
    }

    StructuralEngngModel :: solveYourself();
}



void DDLinearStatic :: solveYourselfAt(TimeStep *tStep)
{


    /**
     * Perform DD
     * Currently this is completely specific, but needs to be generalized
     * by perhaps creating an DD_interface class to do set of functions
     */
    /**************************************************************************/

    int NumberOfDomains = this->giveNumberOfDomains();
    // Loop through all the giveNumberOfDomains
    for (int i = 1; i<= NumberOfDomains; i++) {
        //Domain *domain = this->giveDomain(i);
        /// Create a spatial localizer which in effect has services for locating points in element etc.
        //SpatialLocalizer *sl = domain->giveSpatialLocalizer();
        // Perform DD solution at this time step, solution will depend on quantities in the input file
        /// @todo these have to be defined and initialized earlier
        /// Each domain for the DD case can have only one material... throw error otherwise
        //// Also the DD_domains should be intialized with these materials properties during input
        //// Here i am just using values from input file for algorithmic convenience
        /*
        dd::OofemInterface * interface = new dd::OofemInterface(this);
        */
        dd::Domain dd_domain(70e-3, 0.3, NULL);
        dd::SlipSystem ss0 = dd::SlipSystem(M_PI/6.0, 0.25e-3);
        dd_domain.addSlipSystem(&ss0);

        dd::SlipPlane sp0 = dd::SlipPlane(&dd_domain, &ss0, 1);

		dd::ObstaclePoint o0 = dd::ObstaclePoint(&dd_domain, &sp0, -0.5, 20.0e3);
		dd::ObstaclePoint o1 = dd::ObstaclePoint(&dd_domain, &sp0, 0.5, 20.0e3);
		double e = dd_domain.getModulus();
		double nu = dd_domain.getPassionsRatio();
		double mu = e / (2. * ( 1. + nu));
		double fact = mu * ss0.getBurgersMagnitude() / ( 2 * M_PI * (1. - nu));
		dd::SourcePoint s1 = dd::SourcePoint(&dd_domain, &sp0, 0, 25e-6, fact / 25e-6);
		dd::Vector<2> force, forceGradient;
        dd::Vector<3> stress;

		/*
        dd::DislocationPoint dis0 = dd::DislocationPoint(&dd_domain, &sp0, 0, 1);
        dd::DislocationPoint dis1 = dd::DislocationPoint(&dd_domain, &sp0, 1, -1);

        std::cout << "Dislocation point count: " << sp0.getContainer<dd::DislocationPoint>().size() << "\n";
        
	
	    oofem::FloatArray force_dd;

        dis0.addForceContribution<dd::DislocationPoint>(force, forceGradient, stress);
		
	
	
	// Convert dislocation position to global coordinates 
	//FloatArray FeDislGlobalPos = dis0.getLocation();
	/// Now find stress at this global location 
	/// Step 1
	/// Find Element conatining point 
	//Element *elem = sl->giveElementContainingPoint(FeDislGlobalPos);
	//// Now we need to write 3 routines in oofem to get the strain/stress at this point
	// 1) Convert this global coordinate to element local coordinate
	// 2) Evaluate Stress at this location, there are services in oofem to evaluate stress at a given gp
	//    a) So perhaps it will be easy to convert this to a gp temporarily and then evaluate stress
	//     OR 
	//    b) It is easy to write a service in oofem to evaluate the stress at an arbitrary local point
	/// Output will be FloatArray of size(3) giving quantities exactly the same as stress
	/// So i am using the stress notation to evaluate the force
	/// The total force on any Point is equal to the interaction force + burgers_vec_mag*burg_direction*((stress[1]-stress[2])*cos2i) + stress[3]*sin2i/2.)
	
        std::cout << force[0] << " " << force[1] << "\n";
        */
	
	
    
        
        for( dd_domain.dtNo = 1; dd_domain.dtNo < dd_domain.dtNomax; dd_domain.dtNo++) {
        dd_domain.updateForceCaches();
	int disno = 1;
        for(auto point : dd_domain.getContainer<dd::DislocationPoint>()) {
        	point->sumCaches(force, forceGradient, stress);
        	std::cout << "Cached Force: " << disno << " " << force[0] << " " << force[1] << "\n";
		disno ++;
        }
        s1.spawn(1, 5);
	dd_domain.updateForceCaches(); 
        sp0.moveDislocations(1.0e-11, 1.0e-16);
        
        /*
        for(int bcNo = 1; bcNo <= giveDomain(i)->giveNumberOfBoundaryConditions(); bcNo++) {
        	ManualBoundaryCondition * bc = dynamic_cast<ManualBoundaryCondition *>(giveDomain(i)->giveBc(bcNo));
        	if(bc == nullptr || bc->giveType() != DirichletBT) { continue; }
        	
        	dd::Vector<2> bcContribution;
        	
        	Domain * d = bc->giveDomain();
        	Set * set = d->giveSet(bc->giveSetNumber());
        	
        	
        	for(int nodeNo : set->giveNodeList()) {
                Node * node = static_cast<Node *>(d->giveDofManager(nodeNo));
                for (auto &dofid : bc->giveDofIDs()) {
                    Dof * dof = node->giveDofWithID(dofid);
                    interface->giveNodalBcContribution(node, bcContribution);
                    // TODO: Determine the dimensions without pointer checking
                    double toAdd;
                    if(dof->giveDofID() == D_u) {
                        toAdd = bcContribution[0];
                    }
                    else if(dof->giveDofID() == D_v) {
                        toAdd = bcContribution[1];
                    }
                    else { 
                        OOFEM_ERROR("DOF must be x-disp or y-disp");
                    }
                    bc->addManualValue(dof, toAdd);
                }
            }
            
        	
        	
            std::cout << "BC Contribution: " << bcContribution[0] << " " << bcContribution[1] << "\n";
                      
        }
        
        */
        
		//delete interface;
		} // end dtNo loop
    }





    /**************************************************************************/

    //
    // creates system of governing eq's and solves them at given time step
    //
    // first assemble problem at current time step


    if ( initFlag ) {
#ifdef VERBOSE
        OOFEM_LOG_DEBUG("Assembling stiffness matrix\n");
#endif

        //
        // first step  assemble stiffness Matrix
        //
        stiffnessMatrix.reset( classFactory.createSparseMtrx(sparseMtrxType) );
        if ( !stiffnessMatrix ) {
            OOFEM_ERROR("sparse matrix creation failed");
        }

        stiffnessMatrix->buildInternalStructure( this, 1, EModelDefaultEquationNumbering() );

        this->assemble( *stiffnessMatrix, tStep, TangentAssembler(TangentStiffness),
                        EModelDefaultEquationNumbering(), this->giveDomain(1) );

        initFlag = 0;
    }

#ifdef VERBOSE
    OOFEM_LOG_DEBUG("Assembling load\n");
#endif

    //
    // allocate space for displacementVector
    //
    displacementVector.resize( this->giveNumberOfDomainEquations( 1, EModelDefaultEquationNumbering() ) );
    displacementVector.zero();

    //
    // assembling the load vector
    //
    loadVector.resize( this->giveNumberOfDomainEquations( 1, EModelDefaultEquationNumbering() ) );
    loadVector.zero();
    this->assembleVector( loadVector, tStep, ExternalForceAssembler(), VM_Total,
                          EModelDefaultEquationNumbering(), this->giveDomain(1) );

    //
    // internal forces (from Dirichlet b.c's, or thermal expansion, etc.)
    //
    FloatArray internalForces( this->giveNumberOfDomainEquations( 1, EModelDefaultEquationNumbering() ) );
    internalForces.zero();
    this->assembleVector( internalForces, tStep, InternalForceAssembler(), VM_Total,
                          EModelDefaultEquationNumbering(), this->giveDomain(1) );

    loadVector.subtract(internalForces);

    this->updateSharedDofManagers(loadVector, EModelDefaultEquationNumbering(), ReactionExchangeTag);

    //
    // set-up numerical model
    //
    this->giveNumericalMethod( this->giveMetaStep( tStep->giveMetaStepNumber() ) );

    //
    // call numerical model to solve arose problem
    //
#ifdef VERBOSE
    OOFEM_LOG_INFO("\n\nSolving ...\n\n");
#endif
    NM_Status s = nMethod->solve(*stiffnessMatrix, loadVector, displacementVector);
    if ( !( s & NM_Success ) ) {
        OOFEM_ERROR("No success in solving system.");
    }

    tStep->incrementStateCounter();            // update solution state counter
}


contextIOResultType DDLinearStatic :: saveContext(DataStream *stream, ContextMode mode, void *obj)
//
// saves state variable - displacement vector
//
{
    contextIOResultType iores;
    int closeFlag = 0;
    FILE *file = NULL;

    if ( stream == NULL ) {
        if ( !this->giveContextFile(& file, this->giveCurrentStep()->giveNumber(),
                                    this->giveCurrentStep()->giveVersion(), contextMode_write) ) {
            THROW_CIOERR(CIO_IOERR); // override
        }

        stream = new FileDataStream(file);
        closeFlag = 1;
    }

    if ( ( iores = StructuralEngngModel :: saveContext(stream, mode) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    if ( ( iores = displacementVector.storeYourself(*stream) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    if ( closeFlag ) {
        fclose(file);
        delete stream;
        stream = NULL;
    }

    return CIO_OK;
}


contextIOResultType DDLinearStatic :: restoreContext(DataStream *stream, ContextMode mode, void *obj)
//
// restore state variable - displacement vector
//
{
    contextIOResultType iores;
    int closeFlag = 0;
    int istep, iversion;
    FILE *file = NULL;

    this->resolveCorrespondingStepNumber(istep, iversion, obj);

    if ( stream == NULL ) {
        if ( !this->giveContextFile(& file, istep, iversion, contextMode_read) ) {
            THROW_CIOERR(CIO_IOERR); // override
        }

        stream = new FileDataStream(file);
        closeFlag = 1;
    }

    if ( ( iores = StructuralEngngModel :: restoreContext(stream, mode, obj) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    if ( ( iores = displacementVector.restoreYourself(*stream) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }


    if ( closeFlag ) {
        fclose(file);
        delete stream;
        stream = NULL;
    }

    return CIO_OK;
}


void
DDLinearStatic :: terminate(TimeStep *tStep)
{
    StructuralEngngModel :: terminate(tStep);
    this->printReactionForces(tStep, 1);
    fflush( this->giveOutputStream() );
}


void
DDLinearStatic :: updateDomainLinks()
{
    EngngModel :: updateDomainLinks();
    this->giveNumericalMethod( this->giveCurrentMetaStep() )->setDomain( this->giveDomain(1) );
}


int
DDLinearStatic :: estimateMaxPackSize(IntArray &commMap, DataStream &buff, int packUnpackType)
{
    int count = 0, pcount = 0;
    Domain *domain = this->giveDomain(1);

    if ( packUnpackType == 0 ) { ///@todo Fix this old ProblemCommMode__NODE_CUT value
        for ( int map: commMap ) {
            DofManager *dman = domain->giveDofManager( map );
            for ( Dof *dof: *dman ) {
                if ( dof->isPrimaryDof() && ( dof->__giveEquationNumber() ) ) {
                    count++;
                } else {
                    pcount++;
                }
            }
        }

        // --------------------------------------------------------------------------------
        // only pcount is relevant here, since only prescribed components are exchanged !!!!
        // --------------------------------------------------------------------------------

        return ( buff.givePackSizeOfDouble(1) * pcount );
    } else if ( packUnpackType == 1 ) {
        for ( int map: commMap ) {
            count += domain->giveElement( map )->estimatePackSize(buff);
        }

        return count;
    }

    return 0;
}

} // end namespace oofem
