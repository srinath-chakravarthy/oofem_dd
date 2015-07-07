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

#include "viscoplasticgrad.h"
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
#include <list>
#include <algorithm>
namespace oofem {
REGISTER_Material(ViscoPlasticGrad);

// constructor
ViscoPlasticGrad :: ViscoPlasticGrad(int n, Domain *d) : StructuralMaterial(n, d)
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
ViscoPlasticGrad :: ~ViscoPlasticGrad()
{
    delete linearElasticMaterial;
}

// reads the model parameters from the input file
IRResultType
ViscoPlasticGrad :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                 // required by IR_GIVE_FIELD macro

    result = StructuralMaterial :: initializeFrom(ir);
    if ( result != IRRT_OK ) return result;

    result = linearElasticMaterial->initializeFrom(ir); // takes care of elastic constants
    if ( result != IRRT_OK ) return result;

    G = static_cast< IsotropicLinearElasticMaterial * >(linearElasticMaterial)->giveShearModulus();
    K = static_cast< IsotropicLinearElasticMaterial * >(linearElasticMaterial)->giveBulkModulus();

    IR_GIVE_FIELD(ir, sig0, _IFT_ViscoPlasticGrad_sig0); // uniaxial yield stress

    IR_GIVE_FIELD(ir, sig0, _IFT_ViscoPlasticGrad_sig0); // uniaxial yield stress

    N = 0.;
    IR_GIVE_FIELD(ir, N, _IFT_ViscoPlasticGrad_pow); // Plastic-work hardening exponent

    m0 = 0;
    IR_GIVE_FIELD(ir, m0, _IFT_ViscoPlasticGrad_m); // Rate Sensitivity Exponent

    Cp = 1.0;
    IR_GIVE_OPTIONAL_FIELD(ir, Cp, _IFT_ViscoPlasticGrad_cp); // Scaling factor in ViscoPlasticGrad law
    return IRRT_OK;

    lobs = 0.0;
    IR_GIVE_OPTIONAL_FIELD(ir, lobs, _IFT_ViscoPlasticGrad_lobs); // Intrinsic Length Scale Parameter
    return IRRT_OK;

}

// creates a new material status  corresponding to this class
MaterialStatus *
ViscoPlasticGrad :: CreateStatus(GaussPoint *gp) const
{
    return new ViscoPlasticGradStatus(1, this->giveDomain(), gp);
}

void
ViscoPlasticGrad :: giveRealStressVector_1d(FloatArray &answer,
                                        GaussPoint *gp,
                                        const FloatArray &totalStrain,
                                        TimeStep *tStep)
{
    /// @note: One should obtain the same answer using the iterations in the default implementation (this is verified for this model).
#if 1
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );

    this->performPlasticityReturn(gp, totalStrain);
    answer = status->giveTempEffectiveStress();
    
    // Compute the other components of the strain:
    LinearElasticMaterial *lmat = this->giveLinearElasticMaterial();
    double E = lmat->give('E', gp), nu = lmat->give('n', gp);

    FloatArray strain = status->getTempPlasticStrain();
    strain[0] = totalStrain[0];
    strain[1] -= nu / E * status->giveTempEffectiveStress()[0];
    strain[2] -= nu / E * status->giveTempEffectiveStress()[0];

    status->letTempStrainVectorBe(strain);
    status->letTempStressVectorBe(answer);
#else
    StructuralMaterial :: giveRealStressVector_1d(answer, gp, totalStrain, tStep);
#endif
}

void
ViscoPlasticGrad :: giveRealStressVector_3d(FloatArray &answer,
                                        GaussPoint *gp,
                                        const FloatArray &totalStrain,
                                        TimeStep *tStep)
{
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
    
    // Calculate the gradient of the equivalent stress
    this->performPlasticityReturn(gp, totalStrain);
    answer = status->giveTempEffectiveStress();
    status->letTempStrainVectorBe(totalStrain);
    status->letTempStressVectorBe(answer);
}

double
ViscoPlasticGrad :: computeXita(GaussPoint *gp)
{
    StructuralElement *elem = static_cast< StructuralElement * >( gp->giveElement() );
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
    
    FloatMatrix dN;
    FloatArray XitaVec(3);
    elem->computegradmatrixAt(gp, dN);
    int ielem = elem->giveNumber();
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
      ViscoPlasticGradStatus *ipstatus = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(jGp) );
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
    double xita = XitaVec.computeNorm();
    status->letTempXitaVecBe(XitaVec);
    return xita;
}

void
ViscoPlasticGrad :: giveFirstPKStressVector_3d(FloatArray &answer,
        GaussPoint *gp,
        const FloatArray &totalDefGradOOFEM,
        TimeStep *tStep)
{
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );

    double kappa, dKappa, yieldValue, mi;
    double H = 0.;
    FloatMatrix F, oldF, invOldF;
    FloatArray s;
    
    F.beMatrixForm(totalDefGradOOFEM); //(method assumes full 3D)

    kappa = status->giveCumulativePlasticStrain();
    oldF.beMatrixForm( status->giveFVector() );
    invOldF.beInverseOf(oldF);
    //relative deformation radient
    FloatMatrix f;
    f.beProductOf(F, invOldF);
    //compute elastic predictor
    FloatMatrix trialLeftCauchyGreen, help;

    f.times( 1./cbrt(f.giveDeterminant()) );

    help.beProductOf(f, status->giveTempLeftCauchyGreen());
    trialLeftCauchyGreen.beProductTOf(help, f);
    FloatMatrix E;
    E.beTProductOf(F, F);
    E.at(1, 1) -= 1.0;
    E.at(2, 2) -= 1.0;
    E.at(3, 3) -= 1.0;
    E.times(0.5);

    FloatArray e;
    e.beSymVectorFormOfStrain(E);

    FloatArray leftCauchyGreen;
    FloatArray leftCauchyGreenDev;
    double leftCauchyGreenVol;

    leftCauchyGreen.beSymVectorFormOfStrain(trialLeftCauchyGreen);

    leftCauchyGreenVol = computeDeviatoricVolumetricSplit(leftCauchyGreenDev, leftCauchyGreen);
    FloatArray trialStressDev;
    applyDeviatoricElasticStiffness(trialStressDev, leftCauchyGreenDev, G / 2.);
    s = trialStressDev;

    //check for plastic loading
    double trialS = computeStressNorm(trialStressDev);
    double sigmaY = sig0 + H * kappa;
    //yieldValue = sqrt(3./2.)*trialS-sigmaY;
    yieldValue = trialS - sqrt(2. / 3.) * sigmaY;


    //store deviatoric trial stress(reused by algorithmic stiffness)
    status->letTrialStressDevBe(trialStressDev);
    //the return-mapping algorithm
    double J = F.giveDeterminant();
    mi = leftCauchyGreenVol * G;
    if ( yieldValue > 0 ) {
        //dKappa =sqrt(3./2.)* yieldValue/(H + 3.*mi);
        //kappa = kappa + dKappa;
        //trialStressDev.times(1-sqrt(6.)*mi*dKappa/trialS);
        dKappa = ( yieldValue / ( 2 * mi ) ) / ( 1 + H / ( 3 * mi ) );
        FloatArray n = trialStressDev;
        n.times(2 * mi * dKappa / trialS);
        ////return map
        s.beDifferenceOf(trialStressDev, n);
        kappa += sqrt(2. / 3.) * dKappa;


        //update of intermediate configuration
        trialLeftCauchyGreen.beMatrixForm(s);
        trialLeftCauchyGreen.times(1.0 / G);
        trialLeftCauchyGreen.at(1, 1) += leftCauchyGreenVol;
        trialLeftCauchyGreen.at(2, 2) += leftCauchyGreenVol;
        trialLeftCauchyGreen.at(2, 2) += leftCauchyGreenVol;
        trialLeftCauchyGreen.times(J * J);
    }

    //addition of the elastic mean stress
    FloatMatrix kirchhoffStress;
    kirchhoffStress.beMatrixForm(s);
    kirchhoffStress.at(1, 1) += 1. / 2. *  K * ( J * J - 1 );
    kirchhoffStress.at(2, 2) += 1. / 2. *  K * ( J * J - 1 );
    kirchhoffStress.at(3, 3) += 1. / 2. *  K * ( J * J - 1 );


    FloatMatrix iF, Ep(3, 3), S;
    FloatArray vF, vS, ep;

    //transform Kirchhoff stress into Second Piola - Kirchhoff stress
    iF.beInverseOf(F);
    help.beProductOf(iF, kirchhoffStress);
    S.beProductTOf(help, iF);

    this->computeGLPlasticStrain(F, Ep, trialLeftCauchyGreen, J);

    ep.beSymVectorFormOfStrain(Ep);
    vS.beSymVectorForm(S);
    vF.beVectorForm(F);
    answer.beVectorForm(kirchhoffStress);

    status->setTrialStressVol(mi);
    status->letTempLeftCauchyGreenBe(trialLeftCauchyGreen);
    status->setTempCumulativePlasticStrain(kappa);
    status->letTempStressVectorBe(answer);
    status->letTempStrainVectorBe(e);
    status->letTempPlasticStrainBe(ep);
    status->letTempPVectorBe(answer);
    status->letTempFVectorBe(vF);
}


void
ViscoPlasticGrad :: computeGLPlasticStrain(const FloatMatrix &F, FloatMatrix &Ep, FloatMatrix b, double J)
{
    FloatMatrix I, invB, help;
    I.resize(3, 3);
    I.beUnitMatrix();
    invB.beInverseOf(b);
    help.beTProductOf(F, invB);
    Ep.beProductOf(help, F);
    Ep.times( pow(J, -2. / 3.) );
    Ep.subtract(I);
    Ep.times(1. / 2.);
}


void
ViscoPlasticGrad :: performPlasticityReturn(GaussPoint *gp, const FloatArray &totalStrain)
{
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
    // Local Variables for performing local Newton iterations to integrate Plasticity-Stress strain law
    // For ViscoPlasticGrad Materials
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
    double sigmaY, h, H, dtrialS;
    

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

    // store the deviatoric and trial stress (reused by algorithmic stiffness)

    // New Flow stress at given plasticstrain kappa
    sigmaY = sig0 *pow(1.0 +  Cp*E * kappa/sig0, N);

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
//    while(newton) {
        int knewton = 0;
        double conv = 0.;
        do {
            sigmaY = sig0 *pow(1.0 +  Cp*E * kappa/sig0, N);
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
	//break;
//    }
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
    
    
    applyDeviatoricElasticCompliance(dplStrain, StressDev, 0.5);
    
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
    
    FloatArray printStrain;
    printStrain.beDifferenceOf(totalStrain, plStrain);
    fullStress.printYourself("Stress");
    strain.printYourself("Strain");
    strainIncrement.printYourself("Strain Increment");
    
}

void ViscoPlasticGrad :: computeCumPlastStrain(double &tempKappa, GaussPoint *gp, TimeStep *tStep)
{
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
    tempKappa = status->giveTempCumulativePlasticStrain();
}


void
ViscoPlasticGrad :: give3dMaterialStiffnessMatrix_dPdF(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep)
{
    ///@todo Directly compute dPdF instead.
    FloatMatrix dSdE;
    this->give3dLSMaterialStiffnessMatrix(dSdE, mode, gp, tStep);
    this->give_dPdF_from(dSdE, answer, gp);
}

// returns the consistent (algorithmic) tangent stiffness matrix
void
ViscoPlasticGrad :: give3dMaterialStiffnessMatrix(FloatMatrix &answer,
        MatResponseMode mode,
        GaussPoint *gp,
        TimeStep *tStep)
{
    
    
    if ( mode != TangentStiffness ) {
        return;
    }
    
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
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
    
    double xita = this->computeXita(gp); 
    status->setTempXita(xita);

    
    //answer.printYourself();
    
}

void
ViscoPlasticGrad :: give1dStressStiffMtrx(FloatMatrix &answer,
                                      MatResponseMode mode,
                                      GaussPoint *gp,
                                      TimeStep *tStep)
{
    this->giveLinearElasticMaterial()->give1dStressStiffMtrx(answer, mode, gp, tStep);
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
    double kappa = status->giveCumulativePlasticStrain();
    // increment of cumulative plastic strain as an indicator of plastic loading
    double tempKappa = status->giveTempCumulativePlasticStrain();
    double E = answer.at(1, 1);
    double H = 0.;
    if ( mode != TangentStiffness ) {
        return;
    }


    if ( tempKappa <= kappa ) { // elastic loading - elastic stiffness plays the role of tangent stiffness
        return;
    }


    // === plastic loading ===
    const FloatArray &stressVector = status->giveTempEffectiveStress();
    double stress = stressVector.at(1);
    answer.resize(1, 1);
}

void
ViscoPlasticGrad :: give3dLSMaterialStiffnessMatrix(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep)
{
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
    // start from the elastic stiffness
    double H = 0.;
    FloatMatrix I(6, 6);
    I.at(1, 1) = I.at(2, 2) = I.at(3, 3) = 1;
    I.at(4, 4) = I.at(5, 5) = I.at(6, 6) = 0.5;
    FloatArray delta(6);
    delta.at(1) = delta.at(2) = delta.at(3) = 1;

    FloatMatrix F;
    F.beMatrixForm( status->giveTempFVector() );
    double J = F.giveDeterminant();

    const FloatArray &trialStressDev = status->giveTrialStressDev();
    double trialStressVol = status->giveTrialStressVol();
    double trialS = computeStressNorm(trialStressDev);
    FloatArray n = trialStressDev;
    if ( trialS == 0 ) {
        n.resize(6);
    } else {
        n.times(1 / trialS);
    }


    FloatMatrix C;
    FloatMatrix help;
    help.beDyadicProductOf(delta, delta);
    C = help;
    help.times(-1. / 3.);
    FloatMatrix C1 = I;
    C1.add(help);
    C1.times(2 * trialStressVol);

    FloatMatrix n1, n2;
    n1.beDyadicProductOf(n, delta);
    n2.beDyadicProductOf(delta, n);
    help = n1;
    help.add(n2);
    C1.add(-2. / 3. * trialS, help);
    C.times(K * J * J);

    C.add(-K * ( J * J - 1 ), I);
    C.add(C1);
    //////////////////////////////////////////////////////////////////////////////////////////////////////////

    FloatMatrix invF;
    FloatMatrix T(6, 6);

    invF.beInverseOf(F);
    //////////////////////////////////////////////////
    //first row of pull back transformation matrix
    T.at(1, 1) = invF.at(1, 1) * invF.at(1, 1);
    T.at(1, 2) = invF.at(1, 2) * invF.at(1, 2);
    T.at(1, 3) = invF.at(1, 3) * invF.at(1, 3);
    T.at(1, 4) = 2. * invF.at(1, 2) * invF.at(1, 3);
    T.at(1, 5) = 2. * invF.at(1, 1) * invF.at(1, 3);
    T.at(1, 6) = 2. * invF.at(1, 1) * invF.at(1, 2);
    //second row of pull back transformation matrix
    T.at(2, 1) = invF.at(2, 1) * invF.at(2, 1);
    T.at(2, 2) = invF.at(2, 2) * invF.at(2, 2);
    T.at(2, 3) = invF.at(2, 3) * invF.at(2, 3);
    T.at(2, 4) = 2. * invF.at(2, 2) * invF.at(2, 3);
    T.at(2, 5) = 2. * invF.at(2, 1) * invF.at(2, 3);
    T.at(2, 6) = 2. * invF.at(2, 1) * invF.at(2, 2);
    //third row of pull back transformation matrix
    T.at(3, 1) = invF.at(3, 1) * invF.at(3, 1);
    T.at(3, 2) = invF.at(3, 2) * invF.at(3, 2);
    T.at(3, 3) = invF.at(3, 3) * invF.at(3, 3);
    T.at(3, 4) = 2. * invF.at(3, 2) * invF.at(3, 3);
    T.at(3, 5) = 2. * invF.at(3, 1) * invF.at(3, 3);
    T.at(3, 6) = 2. * invF.at(3, 1) * invF.at(3, 2);
    //fourth row of pull back transformation matrix
    T.at(4, 1) = invF.at(2, 1) * invF.at(3, 1);
    T.at(4, 2) = invF.at(2, 2) * invF.at(3, 2);
    T.at(4, 3) = invF.at(2, 3) * invF.at(3, 3);
    T.at(4, 4) = ( invF.at(2, 2) * invF.at(3, 3) + invF.at(2, 3) * invF.at(3, 2) );
    T.at(4, 5) = ( invF.at(2, 1) * invF.at(3, 3) + invF.at(2, 3) * invF.at(3, 1) );
    T.at(4, 6) = ( invF.at(2, 1) * invF.at(3, 2) + invF.at(2, 2) * invF.at(3, 1) );
    //fifth row of pull back transformation matrix
    T.at(5, 1) = invF.at(1, 1) * invF.at(3, 1);
    T.at(5, 2) = invF.at(1, 2) * invF.at(3, 2);
    T.at(5, 3) = invF.at(1, 3) * invF.at(3, 3);
    T.at(5, 4) = ( invF.at(1, 2) * invF.at(3, 3) + invF.at(1, 3) * invF.at(3, 2) );
    T.at(5, 5) = ( invF.at(1, 1) * invF.at(3, 3) + invF.at(1, 3) * invF.at(3, 1) );
    T.at(5, 6) = ( invF.at(1, 1) * invF.at(3, 2) + invF.at(1, 2) * invF.at(3, 1) );
    //sixth row of pull back transformation matrix
    T.at(6, 1) = invF.at(1, 1) * invF.at(2, 1);
    T.at(6, 2) = invF.at(1, 2) * invF.at(2, 2);
    T.at(6, 3) = invF.at(1, 3) * invF.at(2, 3);
    T.at(6, 4) = ( invF.at(1, 2) * invF.at(2, 3) + invF.at(1, 3) * invF.at(2, 2) );
    T.at(6, 5) = ( invF.at(1, 1) * invF.at(2, 3) + invF.at(1, 3) * invF.at(2, 1) );
    T.at(6, 6) = ( invF.at(1, 1) * invF.at(2, 2) + invF.at(1, 2) * invF.at(2, 1) );
    ///////////////////////////////////////////

    if ( mode != TangentStiffness ) {
        help.beProductTOf(C, T);
        answer.beProductOf(T, help);
        return;
    }


    double kappa = status->giveCumulativePlasticStrain();
    // increment of cumulative plastic strain as an indicator of plastic loading
    double dKappa = sqrt(3. / 2.) * ( status->giveTempCumulativePlasticStrain() - kappa );
    //double dKappa = ( status->giveTempCumulativePlasticStrain() - kappa);
    if ( dKappa <= 0.0 ) { // elastic loading - elastic stiffness plays the role of tangent stiffness
        help.beProductTOf(C, T);
        answer.beProductOf(T, help);
        return;
    }

    // === plastic loading ===
    //dKappa = dKappa*sqrt(3./2.);
    // trial deviatoric stress and its norm


    double beta0, beta1, beta2, beta3, beta4;
    if ( trialS == 0 ) {
        beta1 = 0;
    } else {
        beta1 = 2 * trialStressVol * dKappa / trialS;
    }

    if ( trialStressVol == 0 ) {
        beta0 = 0;
        beta2 = 0;
        beta3 = beta1;
        beta4 = 0;
    } else {
        beta0 = 1 + H / 3 / trialStressVol;
        beta2 = ( 1 - 1 / beta0 ) * 2. / 3. * trialS * dKappa / trialStressVol;
        beta3 = 1 / beta0 - beta1 + beta2;
        beta4 = ( 1 / beta0 - beta1 ) * trialS / trialStressVol;
    }

    FloatMatrix N;
    N.beDyadicProductOf(n, n);
    N.times(-2 * trialStressVol * beta3);

    C1.times(-beta1);
    FloatMatrix mN(3, 3);
    mN.at(1, 1) = n.at(1);
    mN.at(1, 2) = n.at(6);
    mN.at(1, 3) = n.at(5);
    mN.at(2, 1) = n.at(6);
    mN.at(2, 2) = n.at(2);
    mN.at(2, 3) = n.at(4);
    mN.at(3, 1) = n.at(5);
    mN.at(3, 2) = n.at(4);
    mN.at(3, 3) = n.at(3);
    FloatMatrix mN2;
    mN2.beProductOf(mN, mN);

    double volN2 = 1. / 3. * ( mN2.at(1, 1) + mN2.at(2, 2) + mN2.at(3, 3) );
    FloatArray devN2(6);
    devN2.at(1) = mN2.at(1, 1) - volN2;
    devN2.at(2) = mN2.at(2, 2) - volN2;

    devN2.at(3) = mN2.at(3, 3) - volN2;
    devN2.at(4) = mN2.at(2, 3);
    devN2.at(5) = mN2.at(1, 3);
    devN2.at(6) = mN2.at(1, 2);
    FloatMatrix nonSymPart;
    nonSymPart.beDyadicProductOf(n, devN2);
    //symP.beTranspositionOf(nonSymPart);
    //symP.add(nonSymPart);
    //symP.times(1./2.);
    nonSymPart.times(-2 * trialStressVol * beta4);

    C.add(C1);
    C.add(N);
    C.add(nonSymPart);
    help.beProductTOf(C, T);
    answer.beProductOf(T, help);
}


#ifdef __OOFEG
#endif

int
ViscoPlasticGrad :: giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep)
{
    ViscoPlasticGradStatus *status = static_cast< ViscoPlasticGradStatus * >( this->giveStatus(gp) );
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

ViscoPlasticGradStatus :: ViscoPlasticGradStatus(int n, Domain *d, GaussPoint *g) :
    StructuralMaterialStatus(n, d, g), plasticStrain(6), tempPlasticStrain(), trialStressD()
{
  
    // Initialize all the status variables to be reused
    stressVector.resize(6);
    strainVector.resize(6);

    kappa = tempKappa = 0.;
    Xita = tempXita = 0.;
    equivalentStress = tempEquivalentStress = 0.;
    
    XitaVec.resize(3);
    tempXitaVec.resize(3);
    
    effStress.resize(6);
    tempEffStress.resize(6);
    leftCauchyGreen.resize(3, 3);
    leftCauchyGreen.at(1, 1) = leftCauchyGreen.at(2, 2) = leftCauchyGreen.at(3, 3) = 1;
}

ViscoPlasticGradStatus :: ~ViscoPlasticGradStatus()
{ }

void
ViscoPlasticGradStatus :: printOutputAt(FILE *file, TimeStep *tStep)
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
void ViscoPlasticGradStatus :: initTempStatus()
{
    StructuralMaterialStatus :: initTempStatus();

    if ( plasticStrain.giveSize() == 0 ) {
        plasticStrain.resize( StructuralMaterial :: giveSizeOfVoigtSymVector( gp->giveMaterialMode() ) );
        plasticStrain.zero();
    }

    tempPlasticStrain = plasticStrain;
    tempEquivalentStress = equivalentStress;
    tempKappa = kappa;
    
    tempXita = Xita; 
    
    tempXitaVec = tempXita;
    
    trialStressD.clear(); // to indicate that it is not defined yet
    tempLeftCauchyGreen = leftCauchyGreen;
}


// updates internal variables when equilibrium is reached
void
ViscoPlasticGradStatus :: updateYourself(TimeStep *tStep)
{
    StructuralMaterialStatus :: updateYourself(tStep);

    plasticStrain = tempPlasticStrain;
    kappa = tempKappa;
    
    equivalentStress = tempEquivalentStress;
    Xita = tempXita;
    XitaVec = tempXitaVec;
    
    trialStressD.clear(); // to indicate that it is not defined any more
    leftCauchyGreen = tempLeftCauchyGreen;
    /// @todo  Clear all vectors not required except for tangent stiffness

}


// saves full information stored in this status
// temporary variables are NOT stored
contextIOResultType
ViscoPlasticGradStatus :: saveContext(DataStream &stream, ContextMode mode, void *obj)
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
ViscoPlasticGradStatus :: restoreContext(DataStream &stream, ContextMode mode, void *obj)
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
