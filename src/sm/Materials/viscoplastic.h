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

#ifndef viscoplastic_h
#define viscoplastic_h

#include "../sm/Materials/structuralmaterial.h"
#include "../sm/Materials/structuralms.h"
#include "Materials/linearelasticmaterial.h"
#include "dictionary.h"
#include "floatarray.h"
#include "floatmatrix.h"

///@name Input fields for Viscoplastic
//@{
#define _IFT_ViscoPlastic_Name "viscoplastic"
#define _IFT_ViscoPlastic_sig0 "sig0"
#define _IFT_ViscoPlastic_pow "pow"
#define _IFT_ViscoPlastic_m "m"
#define _IFT_ViscoPlastic_cp "cp"
#define _IFT_ViscoPlastic_lobs "lobs"
//@}

namespace oofem {
class GaussPoint;
class Domain;

/**
 * This class implements an isotropic viscoplastic material
 * with Mises yield condition, associated flow rule, with conventional 
 * mechanism based strain gradient plasticity
 * It follows from Needelman Asaro Viscoplastic Model and then 
 */
class ViscoPlastic : public StructuralMaterial
{
protected:
    /// Reference to the basic elastic material.
    LinearElasticMaterial *linearElasticMaterial;

    /// Elastic shear modulus.
    double G;

    /// Elastic bulk modulus.
    double K;

    /// Plastic Work-Hardening exponent.
    double N;

    /// Initial (uniaxial) yield stress.
    double sig0;

    /// Rate sensitivity exponent
    double m0;

    /// Extra Parameter for hardening
    double Cp;
    
    

public:
    ViscoPlastic(int n, Domain * d);
    virtual ~ViscoPlastic();

    void performPlasticityReturn(GaussPoint *gp, const FloatArray &totalStrain);
    
    /** 
     * Computes the local stress gradient at GaussPoint and sets the status variable to xita 
     * In reality plasticityReturn and the stress vector uses the value of (xita/equivalentStress)*lobs 
     * In order for the overloading to work, we need to store the 
     * Stress gradient at an integration point assumes that the stress varies with the same shape functions as the displacement functions
     * Stress gradient is then derivative of the shape function for the element multiplied by the equivalent stress at the integration point
     * @param xita Stress gradient
     * @param gp Integration point where the stress gradient is computed
     * @param tstep Time step when this computation is performed
     * This is overloaded function that also does non-local averaging in the non-local SigmaGP model
    */
    virtual void computeXita(double &xita, GaussPoint *gp, TimeStep *tStep);

    virtual void computeCumPlastStrain(double &kappa, GaussPoint *gp, TimeStep *tStep);

    virtual int hasMaterialModeCapability(MaterialMode mode);

    virtual IRResultType initializeFrom(InputRecord *ir);

    virtual int hasNonLinearBehaviour() { return 1; }
    
    virtual bool isCharacteristicMtrxSymmetric(MatResponseMode rMode) { return false; }

    virtual const char *giveInputRecordName() const { return _IFT_ViscoPlastic_Name; }
    virtual const char *giveClassName() const { return "ViscoPlastic"; }

    /// Returns a reference to the basic elastic material.
    LinearElasticMaterial *giveLinearElasticMaterial() { return linearElasticMaterial; }


    virtual MaterialStatus *CreateStatus(GaussPoint *gp) const;

    virtual void give3dMaterialStiffnessMatrix(FloatMatrix &answer,
                                               MatResponseMode mode,
                                               GaussPoint *gp,
                                               TimeStep *tStep);

//    virtual void give1dStressStiffMtrx(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep); 

/*
    virtual void give3dMaterialStiffnessMatrix_dPdF(FloatMatrix &answer,
                                                    MatResponseMode mode,
                                                    GaussPoint *gp,
                                                    TimeStep *tStep);
*/

    virtual void giveRealStressVector(FloatArray &answer, GaussPoint *gp,
                                      const FloatArray &reducedStrain, TimeStep *tStep);

    virtual void giveRealStressVector_3d(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }
    virtual void giveRealStressVector_PlaneStrain(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }
    virtual void giveRealStressVector_PlaneStress(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }
    virtual void giveRealStressVector_1d(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }



protected:
/*
    void computeGLPlasticStrain(const FloatMatrix &F, FloatMatrix &Ep, FloatMatrix b, double J);

    virtual void give3dLSMaterialStiffnessMatrix(FloatMatrix &answer,
                                                 MatResponseMode mode,
                                                 GaussPoint *gp,
                                                 TimeStep *tStep);
*/
   // virtual void give1dStressStiffMtrx(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep);
    virtual void givePlaneStrainStiffMtrx(FloatMatrix &answer, MatResponseMode mode, GaussPoint *gp, TimeStep *tStep);

    virtual int giveIPValue(FloatArray &answer, GaussPoint *gp, InternalStateType type, TimeStep *tStep);
};

//=============================================================================


class ViscoPlasticStatus : public StructuralMaterialStatus
{
protected:
    /// Plastic strain (initial).
    FloatArray plasticStrain;

    /// Plastic strain (final).
    FloatArray tempPlasticStrain;

    /// Deviatoric trial stress - needed for tangent stiffness.
    FloatArray trialStressD;

    /**************************************************/
    double trialStressV;
    /**************************************************/

    FloatArray effStress;
    
    FloatArray tempEffStress;
    
    double equivalentStress;
    double tempEquivalentStress;
    
    
    double tempDEffTotalStrain;
    
    
    // Total Effective strain (sum of elastic and incremental total)
    double tempEstar;
    
    // Trial Flow stress
    double tempSigmaY;
    
    /// Cumulative plastic strain (initial).
    double kappa;

    /// Cumulative plastic strain (final).
    double tempKappa;
    
    
    // Scalar measure of Gradient of equivalentStress (initial)
    double Xita; 

    
    // Scalar measure of Gradient of equivalentStress (final)
    double tempXita; 
    

    // Vector Gradient of equivalentStress (initial)
    FloatArray XitaVec; 
    
    // Vector Gradient of equivalentStress (final)
    FloatArray tempXitaVec; 


    /************************/
    /******************************/

    /// Left Cauchy-Green deformation gradient (initial).
    FloatMatrix leftCauchyGreen;
    /// Left Cauchy-Green deformation gradient (final).
    FloatMatrix tempLeftCauchyGreen;

public:
    ViscoPlasticStatus(int n, Domain * d, GaussPoint * g);
    virtual ~ViscoPlasticStatus();

    const FloatArray &givePlasticStrain() { return plasticStrain; }

    const FloatArray &giveTrialStressDev() { return trialStressD; }

    /*******************************************/
    double giveTrialStressVol() { return trialStressV; }
    /*******************************************/
    
    double giveCumulativePlasticStrain() { return kappa; }
    double giveTempCumulativePlasticStrain() { return tempKappa; }
    
    double giveEquivalentStress() {return equivalentStress;}
    double giveTempEquivalentStress() {return tempEquivalentStress;}
    
    double givetempDeffTotalstrain() {return tempDEffTotalStrain;}

    double givetempEstar() {return tempEstar;}
    
    double givetempSigmaY() {return tempSigmaY;}
    
    
    double givetempXita() {return tempXita;}
    
    double giveXita() {return Xita;}


    //const FloatMatrix & giveTempLeftCauchyGreen() { return tempLeftCauchyGreen; }
    //const FloatMatrix & giveLeftCauchyGreen() { return leftCauchyGreen; }

    const FloatArray & giveTempEffectiveStress() { return tempEffStress; }
    const FloatArray & giveEffectiveStress() { return effStress; }

    const FloatArray & giveTempXitaVec() { return tempXitaVec; }
    const FloatArray & giveXitaVec() { return XitaVec; }

    
    
    void letTempPlasticStrainBe(FloatArray values) { tempPlasticStrain = std :: move(values); }
    const FloatArray &getTempPlasticStrain() const { return tempPlasticStrain; }

    void letTrialStressDevBe(FloatArray values) { trialStressD = std :: move(values); }

    void letEffectiveStressBe(FloatArray values) { effStress = std :: move(values); }

    void letTempEffectiveStressBe(FloatArray values) { tempEffStress = std :: move(values); }

    void letTempXitaVecBe(FloatArray values) { tempXitaVec = std :: move(values); }


    void setTrialStressVol(double value) { trialStressV = value; }

    void setTempCumulativePlasticStrain(double value) { tempKappa = value; }
    
    void setTempEquivalentStress(double value) { tempEquivalentStress = value; }
    
    void setTempDeffTotalStrain(double value) {tempDEffTotalStrain = value;}
    
    void setTempEstar(double value) {tempEstar = value;}
    
    void setTempSigmaY(double value) {tempSigmaY = value;}
    
    void setTempXita(double value) {tempXita = value;}

    
    /****************************************/
    
    /************************************************/

    //void letTempLeftCauchyGreenBe(FloatMatrix values) { tempLeftCauchyGreen = std :: move(values); }
    //void letLeftCauchyGreenBe(FloatMatrix values) { leftCauchyGreen = std :: move(values); }

    const FloatArray &givePlasDef() { return plasticStrain; }

    virtual void printOutputAt(FILE *file, TimeStep *tStep);

    virtual void initTempStatus();

    virtual void updateYourself(TimeStep *tStep);

    virtual contextIOResultType saveContext(DataStream &stream, ContextMode mode, void *obj = NULL);
    virtual contextIOResultType restoreContext(DataStream &stream, ContextMode mode, void *obj = NULL);

    virtual const char *giveClassName() const { return "ViscoPlasticStatus"; }
};
} // end namespace oofem
#endif // viscoplastic_h
