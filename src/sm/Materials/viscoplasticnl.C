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

#include "viscoplasticnl.h"
#include "../sm/Elements/structuralelement.h"
#include "gausspoint.h"
#include "floatmatrix.h"
#include "floatarray.h"
#include "mathfem.h"
#include "sparsemtrx.h"
#include "nonlocalmaterialext.h"
#include "contextioerr.h"
#include "classfactory.h"
#include "dynamicinputrecord.h"
#include "datastream.h"

namespace oofem {
REGISTER_Material(ViscoPlasticNl);

ViscoPlasticNl :: ViscoPlasticNl(int n, Domain *d) : ViscoPlastic(n, d), StructuralNonlocalMaterialExtensionInterface(d)
{
}


ViscoPlasticNl :: ~ViscoPlasticNl()
{ }


void
ViscoPlasticNl :: giveRealStressVector_3d(FloatArray &answer, GaussPoint *gp,
                                   const FloatArray &totalStrain, TimeStep *tStep)
{
  
    double xita;
    ViscoPlasticStatus *nlstatus = static_cast< ViscoPlasticNlStatus * >( this->giveStatus(gp) );
    // Calculate the gradient of the equivalent stress
    this->computeXita(xita, gp, tStep); 
    nlstatus->setTempXita(xita);   
    this->performPlasticityReturn(gp, totalStrain);
    answer = nlstatus->giveTempEffectiveStress();
    nlstatus->letTempStrainVectorBe(totalStrain);
    nlstatus->letTempStressVectorBe(answer);
}

void 
ViscoPlasticNl :: computeXita(double &xita, GaussPoint *gp, TimeStep *tStep)
{
    /** 
     * Service for computing non-local averaging of the stress gradient
     * Currently only averaging type of 1 is supported
     */
    double nonlocalContribution, nonlocalXita = 0.0;
    ViscoPlasticNlStatus *nonlocStatus, *status = static_cast< ViscoPlasticNlStatus * >( this->giveStatus(gp) );

    this->buildNonlocalPointTable(gp);
    this->updateDomainBeforeNonlocAverage(tStep);

    std :: list< localIntegrationRecord > *list = status->giveIntegrationDomainList();

    for ( auto &lir: *list ) {
        nonlocStatus = static_cast< ViscoPlasticNlStatus * >( this->giveStatus(lir.nearGp) );
        nonlocalContribution = nonlocStatus->giveLocalXitaForAverage();
        nonlocalContribution *= lir.weight;
        nonlocalXita += nonlocalContribution;
    }
    /// nonlocalXita contains now the average stress gradient 
    nonlocalXita *= 1. / status->giveIntegrationScale();

    double stresseq = status->giveEquivalentStress();
    if (stresseq > 1.d-3) {
      nonlocalXita *= 1./stresseq;
    }
    if (this->cl > 0.0) {
      nonlocalXita *= cl;
    } else {
      nonlocalXita = 0.;
    }
    /// nonlocalXita now contains averaged gradient/stresseq*lobs or characteristic length
    /// Now apply constraints that this value is not equal to or greater than 4.0
    if (nonlocalXita >= 3.999) {
      xita = 3.95;
    } else {
      xita = nonlocalXita;
    }
    return;  
}
void
ViscoPlasticNl :: updateBeforeNonlocAverage(const FloatArray &strainVector, GaussPoint *gp, TimeStep *tStep)
{
    /* Implements the service updating local variables in given integration points,
     * which take part in nonlocal average process. Actually, no update is necessary,
     * because the value used for nonlocal averaging is strain vector used for nonlocal secant stiffness
     * computation. It is therefore necessary only to store local strain in corresponding status.
     * This service is declared at StructuralNonlocalMaterial level.
     */

    double xita;
    ViscoPlasticNlStatus *nlstatus = static_cast< ViscoPlasticNlStatus * >( this->giveStatus(gp) );

    this->initTempStatus(gp);
    
    this->computeLocalXita(xita, gp, tStep);
    
    this->performPlasticityReturn(gp, strainVector);
    
    // this->computeLocalCumPlasticStrain(cumPlasticStrain, gp, tStep);
    // standard formulation based on averaging of equivalent strain
    // nlstatus->setLocalCumPlasticStrainForAverage(cumPlasticStrain);
    
    nlstatus->setLocalXitaForAverage(xita);

}


Interface *
ViscoPlasticNl :: giveInterface(InterfaceType type)
{
    if ( type == NonlocalMaterialExtensionInterfaceType ) {
        return static_cast< StructuralNonlocalMaterialExtensionInterface * >(this);
    } else {
        return NULL;
    }
}


IRResultType
ViscoPlasticNl :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                // Required by IR_GIVE_FIELD macro

    result = ViscoPlastic :: initializeFrom(ir);
    if ( result != IRRT_OK ) return result;
    result = StructuralNonlocalMaterialExtensionInterface :: initializeFrom(ir);
    if ( result != IRRT_OK ) return result;

    averType = 5;

    return IRRT_OK;
}


void
ViscoPlasticNl :: giveInputRecord(DynamicInputRecord &input)
{
    StructuralMaterial :: giveInputRecord(input);
    StructuralNonlocalMaterialExtensionInterface :: giveInputRecord(input);

    input.setField(averType, _IFT_ViscoPlasticNl_averagingtype);
}


/*********************************************status**************************************************************/

ViscoPlasticNlStatus :: ViscoPlasticNlStatus(int n, Domain *d, GaussPoint *g) :
    ViscoPlasticStatus(n, d, g), StructuralNonlocalMaterialStatusExtensionInterface()
{
    localCumPlasticStrainForAverage = 0.0;
}


ViscoPlasticNlStatus :: ~ViscoPlasticNlStatus()
{ }


void
ViscoPlasticNlStatus :: printOutputAt(FILE *file, TimeStep *tStep)
{
    StructuralMaterialStatus :: printOutputAt(file, tStep);
    fprintf(file, "status { ");
    fprintf(file, "kappa %f, gradient %f ", this->kappa, this->Xita);
}

void
ViscoPlasticNlStatus :: initTempStatus()
//
// initializes temp variables according to variables form previous equlibrium state.
// builds new crackMap
//
{
    ViscoPlasticStatus :: initTempStatus();
}


void
ViscoPlasticNlStatus :: updateYourself(TimeStep *tStep)
//
// updates variables (nonTemp variables describing situation at previous equilibrium state)
// after a new equilibrium state has been reached
// temporary variables are having values corresponding to newly reched equilibrium.
//
{
    ViscoPlasticStatus :: updateYourself(tStep);
}


contextIOResultType
ViscoPlasticNlStatus :: saveContext(DataStream &stream, ContextMode mode, void *obj)
//
// saves full information stored in this Status
// no temp variables stored
//
{
    contextIOResultType iores;
    // save parent class status
    if ( ( iores = ViscoPlasticStatus :: saveContext(stream, mode, obj) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    //if (!stream.write(&localEquivalentStrainForAverage,1)) THROW_CIOERR(CIO_IOERR);
    return CIO_OK;
}


contextIOResultType
ViscoPlasticNlStatus :: restoreContext(DataStream &stream, ContextMode mode, void *obj)
//
// restores full information stored in stream to this Status
//
{
    contextIOResultType iores;
    // read parent class status
    if ( ( iores = ViscoPlasticStatus :: restoreContext(stream, mode, obj) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    // read raw data
    //if (!stream.read (&localEquivalentStrainForAverage,1)) THROW_CIOERR(CIO_IOERR);

    return CIO_OK;
}


Interface *
ViscoPlasticNlStatus :: giveInterface(InterfaceType type)
{
    if ( type == NonlocalMaterialStatusExtensionInterfaceType ) {
        return static_cast< StructuralNonlocalMaterialStatusExtensionInterface * >(this);
    } else {
        return ViscoPlasticStatus :: giveInterface(type);
    }
}


int
ViscoPlasticNl :: packUnknowns(DataStream &buff, TimeStep *tStep, GaussPoint *ip)
{
    ViscoPlasticNlStatus *nlStatus = static_cast< ViscoPlasticNlStatus * >( this->giveStatus(ip) );

    this->buildNonlocalPointTable(ip);
    this->updateDomainBeforeNonlocAverage(tStep);

    return buff.write( nlStatus->giveLocalCumPlasticStrainForAverage() );
}


int
ViscoPlasticNl :: unpackAndUpdateUnknowns(DataStream &buff, TimeStep *tStep, GaussPoint *ip)
{
    int result;
    ViscoPlasticNlStatus *nlStatus = static_cast< ViscoPlasticNlStatus * >( this->giveStatus(ip) );
    double localCumPlasticStrainForAverage;

    result = buff.read(localCumPlasticStrainForAverage);
    nlStatus->setLocalCumPlasticStrainForAverage(localCumPlasticStrainForAverage);
    return result;
}


int
ViscoPlasticNl :: estimatePackSize(DataStream &buff, GaussPoint *ip)
{
    // Note: nlStatus localStrainVectorForAverage memeber must be properly sized!
    // IDNLMaterialStatus *nlStatus = (IDNLMaterialStatus*) this -> giveStatus (ip);
    return buff.givePackSizeOfDouble(1);
}

} // end namespace oofem
