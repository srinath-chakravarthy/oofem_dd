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

#include "viscoplastic.h"
#include "Materials/isolinearelasticmaterial.h"
#include "../sm/Elements/structuralelement.h"
#include "gausspoint.h"
#include "floatmatrix.h"
#include "floatarray.h"
#include "intarray.h"
#include "mathfem.h"
#include "contextioerr.h"
#include "datastream.h"
#include "classfactory.h"

namespace oofem {
REGISTER_Material(ViscoPlastic);

// constructor
ViscoPlastic :: ViscoPlastic(int n, Domain *d) : StructuralMaterial(n, d)
{
    linearElasticMaterial = new IsotropicLinearElasticMaterial(n, d);
    N = 0.;
    sig0 = 0.;
    G = 0.;
    K = 0.;
    m0 = 0.;
    Cp = 0.;
}

// destructor
ViscoPlastic :: ~ViscoPlastic()
{
    delete linearElasticMaterial;
}

// reads the model parameters from the input file
IRResultType
ViscoPlastic :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                 // required by IR_GIVE_FIELD macro

    result = StructuralMaterial :: initializeFrom(ir);
    if ( result != IRRT_OK ) return result;

    result = linearElasticMaterial->initializeFrom(ir); // takes care of elastic constants
    if ( result != IRRT_OK ) return result;

    G = static_cast< IsotropicLinearElasticMaterial * >(linearElasticMaterial)->giveShearModulus();
    K = static_cast< IsotropicLinearElasticMaterial * >(linearElasticMaterial)->giveBulkModulus();

    IR_GIVE_FIELD(ir, sig0, _IFT_ViscoPlastic_sig0); // uniaxial yield stress

    IR_GIVE_FIELD(ir, sig0, _IFT_ViscoPlastic_sig0); // uniaxial yield stress

    N = 0.;
    IR_GIVE_FIELD(ir, N, _IFT_ViscoPlastic_pow); // Plastic-work hardening exponent

    m0 = 0;
    IR_GIVE_FIELD(ir, m0, _IFT_ViscoPlastic_m); // Rate Sensitivity Exponent

    Cp = 1.0;
    IR_GIVE_OPTIONAL_FIELD(ir, Cp, _IFT_ViscoPlastic_cp); // Scaling factor in ViscoPlastic law

    lobs = 0.0;
    IR_GIVE_OPTIONAL_FIELD(ir, lobs, _IFT_ViscoPlastic_lobs); // Scaling factor in ViscoPlastic law
    return IRRT_OK;
}

// creates a new material status  corresponding to this class
MaterialStatus *
ViscoPlastic :: CreateStatus(GaussPoint *gp) const
{
    return new ViscoPlasticStatus(1, this->giveDomain(), gp);
}


void
ViscoPlastic :: giveRealStressVector_3d(FloatArray &answer,
                                        GaussPoint *gp,
                                        const FloatArray &totalStrain,
                                        TimeStep *tStep)
{
    ViscoPlasticStatus *status = static_cast< ViscoPlasticStatus * >( this->giveStatus(gp) );
    
    /// This is necessary to maintain consistency with non-local model where xita exits
    /// Consequently xita needs to be defined as zero in the local model. 
    status->setTempXita(0.0);
    this->performPlasticityReturn(gp, totalStrain);
    answer = status->giveTempEffectiveStress();
    status->letTempStrainVectorBe(totalStrain);
    status->letTempStressVectorBe(answer);
}

void
ViscoPlastic :: computeXita(double &xita, GaussPoint *gp, TimeStep *tStep)
{
    StructuralElement *elem = static_cast< StructuralElement * >( gp->giveElement() );
    ViscoPlasticStatus *status = static_cast< ViscoPlasticStatus * >( this->giveStatus(gp) );
    
    FloatMatrix dN;
    FloatArray XitaVec(3);
    elem->computegradmatrixAt(gp, dN);
    //int ielem = elem->giveNumber();
    // Number of strain components
    int ncomp = dN.giveNumberOfColumns();
    XitaVec.zero();

    
    if (ncomp == 1) {
        XitaVec.at(2) = 0.;
        XitaVec.at(3) = 0.;
    }
    else if (ncomp == 2) {
        XitaVec.at(3) = 0.;
    }
    
    IntegrationRule *iRule;
    iRule = elem->giveDefaultIntegrationRulePtr();
    FloatArray gpCoords, jGpCoords;
    std :: list < double > seq;
    int k = 0;
    for ( GaussPoint *jGp: *iRule ) {
      ViscoPlasticStatus *ipstatus = static_cast< ViscoPlasticStatus * >( this->giveStatus(jGp) );
      seq.push_back(ipstatus->giveEquivalentStress());
      k ++;
    }
    //double *stresseq = new double[seq->size()];
    int ss = seq.size();
    FloatArray stresseq(ss);
    std::copy(seq.begin(), seq.end(), stresseq.begin());
    //seq->clear();
    for (int i=1; i<= dN.giveNumberOfRows(); i++) {
      for (int j=1; j<=ncomp; j++){
	XitaVec.at(j) += dN.at(i,j)*stresseq.at(i);
      }
    }
    xita = XitaVec.computeNorm();
    status->letTempXitaVecBe(XitaVec);
    status->setTempXita(xita);
}




void
ViscoPlastic :: performPlasticityReturn(GaussPoint *gp, const FloatArray &totalStrain)
{
    ViscoPlasticStatus *status = static_cast< ViscoPlasticStatus * >( this->giveStatus(gp) );
    // Local Variables for performing local Newton iterations to integrate Plasticity-Stress strain law
    // For ViscoPlastic Materials
    const double NRtol = 1.e-6; // Tolerance of Newton-Raphson locally
    
    const double zero = 1.e-30; // Tolerance zero values

    double kappa; // Total Effective Plastic strain
    double kappa_init; // Stores incoming Total Effective Plastic strain
    double yieldValue; // Temporary Variable
    double dKappa; // Incremental Effective Plastic Strain
    double E; // Youngs Modulus

    double DEE; // Incremental Effective Total Strain

    // Temporary variables during integration
    double RHS, Bot, pama, Ratio;
    double sigmaY, h, dtrialS;
    

    double trialS; // Effective Stress (J2)

    E = static_cast< IsotropicLinearElasticMaterial * >(linearElasticMaterial)->giveYoungsModulus();

    FloatArray plStrain, dplStrain;
    FloatArray fullStress, StressDev;
    double StressVol;
    // get the initial plastic strain and initial kappa from the status
    plStrain = status->givePlasticStrain();
    kappa = status->giveCumulativePlasticStrain();
    kappa_init = kappa;

    // Strain the end of the previous step --> total Strain vector
    const FloatArray strain = status->giveStrainVector();
    FloatArray strain_dev;
    double strainVol;

    strainVol = computeDeviatoricVolumetricSplit(strain_dev, strain);
    // Stress at the end of the previous step
    const FloatArray stress = status->giveStressVector();

    // Strain increment Quantities
    FloatArray strainIncrement = status->giveStrainVector();
    strainIncrement.zero();
    strainIncrement.beDifferenceOf(totalStrain, status->giveStrainVector());
    double strainIncrementVol;
    FloatArray strainIncrementDev;
    strainIncrementVol = computeDeviatoricVolumetricSplit(strainIncrementDev, strainIncrement);

    // Equivalent strain increment

    DEE = sqrt ( 2. / 3. ) * computeStrainNorm(strainIncrementDev);

    // store the incremental effective deviatoric Strain
    status->setTempDeffTotalStrain(DEE);

    
    
    // Stress Quantities

    StressVol = computeDeviatoricVolumetricSplit(StressDev, stress);

    // J2 equivalent Stress
    trialS = sqrt( 3. / 2.) * computeStressNorm(StressDev);

    //status->setTempEquivalentStress(trialS);
    // Elastic Strain at previous step ---> computed from Stress
    FloatArray elStrainDev;
    elStrainDev.zero();
    double elStrainVol = strainVol;
    applyDeviatoricElasticCompliance(elStrainDev, StressDev, G);

    double totalStrainVol = elStrainVol + strainIncrementVol;
    // StrainTildeDev is the sum of Deviatoric elastic strain at previous and current total
    FloatArray StrainTildeDev = elStrainDev;
    StrainTildeDev.add(strainIncrementDev);

    // Estar is the equivalent StrainTildeDev
    double EStar = sqrt (2. / 3.) * computeStrainNorm(StrainTildeDev);
    //stor Estar and sigmaY in status
    status->setTempEstar(EStar);

    /// Stress-Gradient plasticity parameter non-local model 
    double xita = status->givetempXita();
    
    /// @todo overload computeXita method to include non-local average
    /// Overloaded computeXita function returns non-zero value only if non-local 
    /// It contains the effective value of xita, which is (stress_gradient/equivalent_stress)*lobs along with constraints
    

    
    // store the deviatoric and trial stress (reused by algorithmic stiffness)

    // New Flow stress at given plasticstrain kappa
    sigmaY = sig0 *pow(1.0 +  Cp*E * kappa/sig0, N)/(1.-0.25*xita);

    yieldValue = trialS/sigmaY;
    pama = pow ( yieldValue, m0);

    dKappa = DEE*pama;
    kappa = kappa_init + dKappa;
    trialS = 3. * G * (EStar - dKappa);
    
    bool newton;
    if (fabs(trialS) < zero){
      trialS = zero;
      newton = false;
    }
    else{
      newton = true;
    }
    while(newton) {
        int knewton = 0;
        double conv = 0.;
        do {
            sigmaY = sig0 *pow(1.0 +  Cp*E * kappa/sig0, N)/(1.-0.25*xita);
            yieldValue = trialS/sigmaY;
            pama = pow ( yieldValue, m0);
            RHS = 3. * G * ( EStar - DEE * pama) - trialS;
            h = DEE * pow (( trialS / sigmaY ), ( m0 - 1. )) * m0 / sigmaY;
            Bot = 1. + 3. * G * h;
            dtrialS = RHS / Bot;
            trialS += dtrialS;
            if (fabs(trialS) < zero) {
                trialS = zero;
            }
            dKappa = EStar - trialS / (3. * G);
            kappa = kappa_init + dKappa;
            knewton += 1;
            conv = fabs(dtrialS/trialS);
        } while (fabs(dtrialS) > 1.0e-12 && conv > NRtol);
	newton = false;
	break;
    }
    // Calculate new Stress
    double temp = dKappa / trialS;
    if (fabs(dKappa) <= zero && trialS <= zero) {
        temp = zero;
    }

    if (temp > 1.e50) {
        temp = 1.e50;
    }
    Ratio = G / ( 1. + 3. * G * temp);

    StressVol = 3. * K * totalStrainVol;
    // Apply Ratio to StrainTildeDev
    applyDeviatoricElasticStiffness(StressDev, StrainTildeDev, Ratio);
    computeDeviatoricVolumetricSum(fullStress, StressDev, StressVol);

    // Calculate Plastic Strain and incremental Plastic Strain
    
    
    //applyDeviatoricElasticCompliance(dplStrain, StressDev, 0.5);
    //dplStrain = StressDev;
    dplStrain.beScaled(0.5, StressDev);
    
    dplStrain.times(dKappa * 3. / trialS);
    
    plStrain.add(dplStrain);


    status->letTrialStressDevBe(StressDev);
    status->setTrialStressVol(StressVol);

    status->setTempSigmaY(sigmaY);

    //store the equivalent Stress in status
    status->setTempEquivalentStress(trialS);

    // store the effective stress in status
    status->letTempEffectiveStressBe(fullStress);

    // store the effective stress in status
    status->letTempPlasticStrainBe(plStrain);
    status->setTempCumulativePlasticStrain(kappa);
    /*
    FloatArray printStrain;
    printStrain.beDifferenceOf(totalStrain, plStrain);
    fullStress.printYourself("Stress");
    strain.printYourself("Strain");
    strainIncrement.printYourself("Strain Increment");
    */
}

void ViscoPlastic :: computeCumPlastStrain(double &tempKappa, GaussPoint *gp, TimeStep *tStep)
{
    ViscoPlasticStatus *status = static_cast< ViscoPlasticStatus * >( this->giveStatus(gp) );
    tempKappa = status->giveTempCumulativePlasticStrain();
}


// returns the consistent (algorithmic) tangent stiffness matrix
void
ViscoPlastic :: give3dMaterialStiffnessMatrix(FloatMatrix &answer,
        MatResponseMode mode,
        GaussPoint *gp,
        TimeStep *tStep)
{
    
    
    if ( mode != TangentStiffness ) {
        return;
    }
    
    ViscoPlasticStatus *status = static_cast< ViscoPlasticStatus * >( this->giveStatus(gp) );
    /*
    FloatArray tempStrain = status->giveTempStrainVector();
    FloatArray sig(6), strain(6);
    if (tempStrain.at(1) == 0.){
      tempStrain.resize(6);
    }
    this->giveRealStressVector_3d(sig, gp, tempStrain, tStep);
    */
    double kappa = status->giveCumulativePlasticStrain();
    double tempKappa = status->giveTempCumulativePlasticStrain();
    // increment of cumulative plastic strain as an indicator of plastic loading
    double dKappa = tempKappa - kappa;
    /*
    if ( dKappa <= 0.0 ) { // elastic loading - elastic stiffness plays the role of tangent stiffness
        return;
    }
    */
    // === plastic loading ===

    double EStar = status->givetempEstar();    
    
    double DEE = status->givetempDeffTotalstrain();
    
    double trialS = status->giveTempEquivalentStress();
        
    //double sigmaY = sig0 * pow(1. +  Cp * E * kappa / sig0, N);
    double sigmaY = status->givetempSigmaY();
    
    // Calculate Q, R, h parameters
    double Q, R;
    double h = DEE * pow((trialS / sigmaY),(m0 - 1.))*m0/sigmaY;

    if ( fabs(trialS) < 1.e-30 || fabs(EStar) < 1.e-30){
      Q = 2.*G;
      R = 0.;
    }
    else {
      Q = 2. / 3. * trialS/EStar;
      R = 1. / (trialS * EStar) * (h - dKappa/trialS) * (3. * G) / (1. + 3. * G * h);
    }

    // Stored Deviatoric Trial stress state
    FloatArray fullStressDev = status->giveTrialStressDev();

    
    // Create Stiffness matrix term by term
    // one correction term
    FloatMatrix stiffnessCorrection(6, 6);
    stiffnessCorrection.beDyadicProductOf(fullStressDev, fullStressDev);
    double factor1 = -1.* R; 
    stiffnessCorrection.times(factor1);
    
    answer.add(stiffnessCorrection);

    // another correction term
    stiffnessCorrection.bePinvID();
    double factor2 = Q;
    
    answer.add(factor2, stiffnessCorrection);
    
    stiffnessCorrection.zero();
    stiffnessCorrection.at(1,1) = stiffnessCorrection.at(1,2) = stiffnessCorrection.at(1,3) = K;
    stiffnessCorrection.at(2,1) = stiffnessCorrection.at(2,2) = stiffnessCorrection.at(2,3) = K;
    stiffnessCorrection.at(3,1) = stiffnessCorrection.at(3,2) = stiffnessCorrection.at(3,3) = K;

    answer.add(stiffnessCorrection);
    //answer.printYourself();
    
}

#ifdef __OOFEG
#endif

int
ViscoPlastic :: giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep)
{
    ViscoPlasticStatus *status = static_cast< ViscoPlasticStatus * >( this->giveStatus(gp) );
    if ( type == IST_PlasticStrainTensor ) {
        answer = status->givePlasDef();
        return 1;
    } else if ( type == IST_MaxEquivalentStrainLevel ) {
        answer.resize(1);
        answer.at(1) = status->giveCumulativePlasticStrain();
        return 1;
    } else {
        return StructuralMaterial :: giveIPValue(answer, gp, type, tStep);
    }
}

//=============================================================================

ViscoPlasticStatus :: ViscoPlasticStatus(int n, Domain *d, GaussPoint *g) :
    StructuralMaterialStatus(n, d, g), plasticStrain(6), tempPlasticStrain(), trialStressD()
{
    stressVector.resize(6);
    strainVector.resize(6);

    kappa = tempKappa = 0.;
    effStress.resize(6);
    tempEffStress.resize(6);
    /*
    leftCauchyGreen.resize(3, 3);
    leftCauchyGreen.at(1, 1) = leftCauchyGreen.at(2, 2) = leftCauchyGreen.at(3, 3) = 1;
    */
}

ViscoPlasticStatus :: ~ViscoPlasticStatus()
{ }

void
ViscoPlasticStatus :: printOutputAt(FILE *file, TimeStep *tStep)
{
    StructuralMaterialStatus :: printOutputAt(file, tStep);

    fprintf(file, "              plastic  ");
    for ( auto &val : this->plasticStrain ) {
        fprintf(file, "%.4e ", val);
    }
    fprintf(file, "\n");

    fprintf(file, "status { ");
    /*
     * // this would not be correct, since the status is already updated and kappa is now the "final" value
     * if ( tempKappa > kappa ) {
     * fprintf(file, " Yielding, ");
     * } else {
     * fprintf(file, " Unloading, ");
     * }
     */

    /*********************************************************************************/


    /********************************************************************************/
    /*
     * // print the plastic strain
     *  n = plasticStrain.giveSize();
     *  fprintf(file, " plastic_strains ");
     *  for ( i = 1; i <= n; i++ ) {
     *      fprintf( file, " %.4e", plasticStrain.at(i) );
     *  }
     */
    // print the cumulative plastic strain
    fprintf(file, ", kappa ");
    fprintf(file, " %.4e", kappa);

    fprintf(file, "}\n");
    /*
     * //print Left Cauchy - Green deformation tensor
     * fprintf(file," left Cauchy Green");
     * fprintf( file, " %.4e",tempLeftCauchyGreen.at(1,1) );
     * fprintf( file, " %.4e",tempLeftCauchyGreen.at(2,2) );
     * fprintf( file, " %.4e",tempLeftCauchyGreen.at(3,3) );
     * fprintf( file, " %.4e",tempLeftCauchyGreen.at(2,3) );
     * fprintf( file, " %.4e",tempLeftCauchyGreen.at(1,3) );
     * fprintf( file, " %.4e",tempLeftCauchyGreen.at(1,2) );
     *
     * //print deformation gradient
     * fprintf(file," Deformation Gradient");
     * fprintf( file, " %.4e",tempDefGrad.at(1,1) );
     * fprintf( file, " %.4e",tempDefGrad.at(1,2) );
     * fprintf( file, " %.4e",tempDefGrad.at(1,3) );
     * fprintf( file, " %.4e",tempDefGrad.at(2,1) );
     * fprintf( file, " %.4e",tempDefGrad.at(2,2) );
     * fprintf( file, " %.4e",tempDefGrad.at(2,3) );
     * fprintf( file, " %.4e",tempDefGrad.at(3,1) );
     * fprintf( file, " %.4e",tempDefGrad.at(3,2) );
     * fprintf( file, " %.4e",tempDefGrad.at(3,3) );
     */

    fprintf(file, "}\n");
}


// initializes temporary variables based on their values at the previous equlibrium state
void ViscoPlasticStatus :: initTempStatus()
{
    StructuralMaterialStatus :: initTempStatus();

   
    if ( plasticStrain.giveSize() == 0 ) {
        plasticStrain.resize( StructuralMaterial :: giveSizeOfVoigtSymVector( gp->giveMaterialMode() ) );
        plasticStrain.zero();
    }
   
    tempPlasticStrain = plasticStrain;
    tempKappa = kappa;

    tempXita = Xita; 
    tempXitaVec = tempXita;
    tempEquivalentStress = equivalentStress;

   
    trialStressD.clear(); // to indicate that it is not defined yet
    
    //tempLeftCauchyGreen = leftCauchyGreen;
}


// updates internal variables when equilibrium is reached
void
ViscoPlasticStatus :: updateYourself(TimeStep *tStep)
{
    StructuralMaterialStatus :: updateYourself(tStep);

    plasticStrain = tempPlasticStrain;
    kappa = tempKappa;
    
    /// Stress gradient parameters
    equivalentStress = tempEquivalentStress;
    Xita = tempXita;
    XitaVec = tempXitaVec;

    
    trialStressD.clear(); // to indicate that it is not defined any more
    //leftCauchyGreen = tempLeftCauchyGreen;
}


// saves full information stored in this status
// temporary variables are NOT stored
contextIOResultType
ViscoPlasticStatus :: saveContext(DataStream &stream, ContextMode mode, void *obj)
{
    contextIOResultType iores;

    // save parent class status
    if ( ( iores = StructuralMaterialStatus :: saveContext(stream, mode, obj) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    // write raw data

    // write plastic strain (vector)
    if ( ( iores = plasticStrain.storeYourself(stream) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    // write cumulative plastic strain (scalar)
    if ( !stream.write(kappa) ) {
        THROW_CIOERR(CIO_IOERR);
    }

    return CIO_OK;
}



contextIOResultType
ViscoPlasticStatus :: restoreContext(DataStream &stream, ContextMode mode, void *obj)
//
// restores full information stored in stream to this Status
//
{
    contextIOResultType iores;

    // read parent class status
    if ( ( iores = StructuralMaterialStatus :: restoreContext(stream, mode, obj) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    // read plastic strain (vector)
    if ( ( iores = plasticStrain.restoreYourself(stream) ) != CIO_OK ) {
        THROW_CIOERR(iores);
    }

    // read cumulative plastic strain (scalar)
    if ( !stream.read(kappa) ) {
        THROW_CIOERR(CIO_IOERR);
    }

    return CIO_OK; // return succes
}
} // end namespace oofem
