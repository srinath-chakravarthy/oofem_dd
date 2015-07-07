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
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef anisodamagemodel_h
#define anisodamagemodel_h

// this turns on or off a bunch of internal variables
// that allow tracing the distribution of dissipated energy
// (can be turned off if such information is not needed)
#define keep_track_of_dissipated_energy

#include "material.h"
#include "../sm/Materials/structuralmaterial.h"
#include "isolinearelasticmaterial.h"
#include "../sm/Materials/structuralms.h"
#include "Materials/ConcreteMaterials/idm1.h"

///@name Input fields for AnisotropicDamageMaterial
//@{
// @todo add input parametres which are read from input file here
#define _IFT_AnisotropicDamageMaterial_Name "adm"
#define _IFT_AnisotropicDamageMaterial_equivStrainType "equivstraintype"
#define _IFT_AnisotropicDamageMaterial_damageLawType "damlaw"
#define _IFT_AnisotropicDamageMaterial_kappa0 "kappa0"
#define _IFT_AnisotropicDamageMaterial_kappaf "kappaf"
#define _IFT_AnisotropicDamageMaterial_aA "aa"
//@}

namespace oofem {
class GaussPoint;

/**
 * This class implements associated Material Status to AnisotropicDamageMaterial.
 * Stores a damage tensor and hardening variable (and possible extra information).
 */
class AnisotropicDamageMaterialStatus : public StructuralMaterialStatus
{
protected:
    /// Scalar measure of the largest strain level ever reached in material.
    double kappa;
    /// Non-equilibrated scalar measure of the largest strain level.
    double tempKappa;
    /// Second order damage tensor
    FloatMatrix damage;
    /// Non-equilibrated second order damage tensor
    FloatMatrix tempDamage;
    /// Out-of-plane value for 2dPlaneStress mode
    double tempStrainZ;
    /// Non-equilibrated out-of-plane value for 2dPlaneStress mode
    double strainZ;
    /// This flag turns into 1 and remains 1 when the trace of the damage tensor is >1 in compression (tr(strainTensor)<0)
    int flag;
    int tempFlag;
    double storedFactor;
    double tempStoredFactor;

#ifdef keep_track_of_dissipated_energy
    /// Density of total work done by stresses on strain increments.
    double stressWork;
    /// Non-equilibrated density of total work done by stresses on strain increments.
    double tempStressWork;
    /// Density of dissipated work.
    double dissWork;
    /// Non-equilibrated density of dissipated work.
    double tempDissWork;
#endif

public:
    /// Constructor
    AnisotropicDamageMaterialStatus(int n, Domain *d, GaussPoint *g);
    /// Destructor
    virtual ~AnisotropicDamageMaterialStatus();

    virtual void printOutputAt(FILE *file, TimeStep *tStep);

    /// Returns the last equilibrated scalar measure of the largest strain level.
    double giveKappa() { return kappa; }
    /// Returns the temp. scalar measure of the largest strain level.
    double giveTempKappa() { return tempKappa; }
    /// Sets the temp scalar measure of the largest strain level to given value.
    void setTempKappa(double newKappa) { tempKappa = newKappa; }
    /// Returns the last equilibrated second order damage tensor.
    const FloatMatrix &giveDamage() { return damage; }
    /// Returns the temp. second order damage tensor.
    const FloatMatrix &giveTempDamage() { return tempDamage; }
    /// Assigns temp. damage tensor to given tensor d
    void setTempDamage(const FloatMatrix &d) { tempDamage = d; }
    /// Returns the last equilibrated scalar measure of the out-of-plane strain to given value (for 2dPlaneStress mode).
    double giveStrainZ() { return strainZ; }
    /// Returns the temp scalar measure of the out-of-plane strain to given value (for 2dPlaneStress mode).
    double giveTempStrainZ() {  return tempStrainZ;     }
    /// Sets the temp scalar measure of the out-of-plane strain to given value (for 2dPlaneStress mode).
    void setTempStrainZ(double newStrainZ) { tempStrainZ = newStrainZ; }
    /// Returns the value of the flag.
    int giveFlag() { return flag; }
    /// Sets the value of the temporary value of flag.
    void setTempFlag(int newflag) { tempFlag = newflag; }
    /// Returns the value of the temporary value of flag.
    int giveTempFlag() { return tempFlag; }
    /// Returns the last Stored Factor.
    double giveStoredFactor() { return storedFactor; }
    /// Sets the Stored Factor to given value .
    void setStoredFactor(double newStoredFactor) { storedFactor = newStoredFactor; }
    /// Returns the last Temp Stored Factor.
    double giveTempStoredFactor() { return tempStoredFactor; }
    /// Sets the Temp Stored Factor to given value .
    void setTempStoredFactor(double newTempStoredFactor) { tempStoredFactor = newTempStoredFactor; }



#ifdef keep_track_of_dissipated_energy
    /// Returns the density of total work of stress on strain increments.
    double giveStressWork() { return stressWork; }
    /// Returns the temp density of total work of stress on strain increments.
    double giveTempStressWork() { return tempStressWork; }
    /// Sets the density of total work of stress on strain increments to given value.
    void setTempStressWork(double w) { tempStressWork = w; }
    /// Returns the density of dissipated work.
    double giveDissWork() { return dissWork; }
    /// Returns the density of temp dissipated work.
    double giveTempDissWork() { return tempDissWork; }
    /// Sets the density of dissipated work to given value.
    void setTempDissWork(double w) { tempDissWork = w; }
    /// Computes the increment of total stress work and of dissipated work.
    void computeWork(GaussPoint *gp);
#endif

    // definition
    virtual const char *giveClassName() const { return "AnisotropicDamageMaterialModelStatus"; }

    virtual void initTempStatus();
    virtual void updateYourself(TimeStep *tStep);

    virtual contextIOResultType saveContext(DataStream &stream, ContextMode mode, void *obj = NULL);
    virtual contextIOResultType restoreContext(DataStream &stream, ContextMode mode, void *obj = NULL);
};


/**
 * Class representing anisotropic damage model.
 * Based on the paper : "Nonlocal anisotropic damage model and related
 * computational aspects for quasi-brittle materials" by R. Desmorat, F. Gatuingt, and F. Ragueneau
 * Plane stress case implemented by the algorithm described in a report by M. Jirasek & F. Suarez, 25 April 2014
 * @author : Martin Horak : nitramkaroh@seznam.cz
 * @author : Fernando Suarez : fernando.suarez@fsv.cvut.cz
 * @author : Milan Jirasek : milan.jirasek@fsv.cvut.cz
 */
class AnisotropicDamageMaterial : public StructuralMaterial
{
protected:

    /// Reference to bulk (undamaged) material
    IsotropicLinearElasticMaterial *linearElasticMaterial;
    /// Young's modulus
    double E;
    /// Poisson's ratio
    double nu;
    /// Damage threshold kappa0, as defined in the paper mentioned above.
    double kappa0;
    /// Damage parameter kappa_f (in the paper denoted as "a")
    double kappaf;
    /// Damage parameter a*A, needed to obtain Kappa(trD), according to eq. 33 in the paper mentioned above.
    double aA;
    /// Type characterizing the algorithm used to compute equivalent strain measure.
    enum EquivStrainType { EST_Unknown, EST_Mazars, EST_Rankine_Smooth, EST_Rankine_Standard, EST_ElasticEnergy, EST_ElasticEnergyPositiveStress, EST_ElasticEnergyPositiveStrain, EST_Mises, EST_Griffith };
    /// Parameter specifying the definition of equivalent strain.
    EquivStrainType equivStrainType;
    /// Type characterizing the damage law.
    enum DamLawType { DLT_Unknown, DLT_Desmorat1, DLT_Desmorat2, DLT_Linear, DLT_Exponential };
    /// Parameter specifying the damage law.
    DamLawType damageLawType;

public:
    /// Constructor
    AnisotropicDamageMaterial(int n, Domain *d);
    /// Destructor
    virtual ~AnisotropicDamageMaterial();

    virtual int hasNonLinearBehaviour() { return 1; }
    virtual int hasMaterialModeCapability(MaterialMode mode);

    // identification and auxiliary functions
    virtual const char *giveClassName() const { return "AnisotropicDamageMaterial"; }
    virtual const char *giveInputRecordName() const { return _IFT_AnisotropicDamageMaterial_Name; }

    /// Returns reference to undamaged (bulk) material
    IsotropicLinearElasticMaterial *giveLinearElasticMaterial() { return linearElasticMaterial; }

    /// Plane-stress version of the stress evaluation algorithm
    virtual void giveRealStressVector_PlaneStress(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep);
    void computePrincValDir2D(double &D1, double &D2, double &c, double &s, double Dx, double Dy, double Dxy);
    bool checkPrincVal2D(double Dx, double Dy, double Dxy);
    void computeDamage(FloatMatrix &tempDamage, const FloatMatrix &damage, double kappa, double eps1, double eps2, double ceps, double seps, double epsZ);
    double computeTraceD(double equivStrain);
    double computeOutOfPlaneStrain(const FloatArray &inplaneStrain, const FloatMatrix &dam, bool tens_flag);
    double computeDimensionlessOutOfPlaneStress(const FloatArray &inplaneStrain, double epsZ, const FloatMatrix &dam);
    void computeInplaneStress(FloatArray &inplaneStress, const FloatArray &inplaneStrain, double epsZ, const FloatMatrix &dam);


    /// Obtains the proportion of the damage tensor that is needed to get the first eigenvalue equal to the damage threshold
    double obtainAlpha1(FloatMatrix tempDamageTensor, double deltaLambda, FloatMatrix positiveStrainTensor, double damageThreshold);
    /// Obtains the proportion of the damage tensor that is needed to get the second eigenvalue equal to the damage threshold
    double obtainAlpha2(FloatMatrix tempDamageTensor, double deltaLambda, FloatMatrix positiveStrainTensor, FloatMatrix ProjMatrix, double damageThreshold);
    /// Obtains the proportion of the damage tensor that is needed to get the third eigenvalue equal to the damage threshold
    double obtainAlpha3(FloatMatrix tempDamageTensor, double deltaLambda, FloatMatrix positiveStrainTensor, FloatArray vec3, double damageThreshold);

    double checkSymmetry(FloatMatrix matrix);

    void correctBigValues(FloatMatrix &matrix);

    double computeTraceD(FloatMatrix tempDamageTensor, FloatMatrix strainTensor, GaussPoint *gp);

    double computeCorrectionFactor(FloatMatrix tempDamageTensor, FloatMatrix strainTensor, GaussPoint *gp);

    virtual void give3dMaterialStiffnessMatrix(FloatMatrix &answer,
                                               MatResponseMode mode,
                                               GaussPoint *gp,
                                               TimeStep *tStep);

    virtual void giveRealStressVector(FloatArray &answer,  GaussPoint *gp,
                                      const FloatArray &reducedStrain, TimeStep *tStep);

    virtual void giveRealStressVector_3d(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }
    virtual void giveRealStressVector_PlaneStrain(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }
    virtual void giveRealStressVector_StressControl(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, const IntArray &strainControl, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }
    virtual void giveRealStressVector_1d(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedE, TimeStep *tStep)
    { this->giveRealStressVector(answer, gp, reducedE, tStep); }

    virtual int giveIPValue(FloatArray &answer, GaussPoint *aGaussPoint, InternalStateType type, TimeStep *atTime);

    //    virtual InternalStateValueType giveIPValueType(InternalStateType type);
    //virtual void giveThermalDilatationVector(FloatArray &answer, GaussPoint *, TimeStep *);

    /**
     * Computes the equivalent strain measure from given strain vector (full form).
     * @param[out] kappa Return parameter, containing the corresponding equivalent strain.
     * @param strain Total strain vector in full form.
     * @param gp Integration point.
     * @param tStep Time step.
     */
    virtual void computeEquivalentStrain(double &kappa, const FloatArray &strain, GaussPoint *gp, TimeStep *tStep);

    virtual IRResultType initializeFrom(InputRecord *ir);
    virtual void giveInputRecord(DynamicInputRecord &input);
    virtual void computeDamageTensor(FloatMatrix &answer, GaussPoint *gp,
                                     const FloatArray &totalStrain, double equivStrain,
                                     TimeStep *atTime);

    MaterialStatus *CreateStatus(GaussPoint *gp) const { return new AnisotropicDamageMaterialStatus(1, domain, gp); }

protected:


    virtual void givePlaneStressStiffMtrx(FloatMatrix &answer, MatResponseMode mmode,
                                          GaussPoint *gp,
                                          TimeStep *tStep);

    virtual void givePlaneStrainStiffMtrx(FloatMatrix &answer, MatResponseMode mmode,
                                          GaussPoint *gp,
                                          TimeStep *tStep);

    virtual void give1dStressStiffMtrx(FloatMatrix &answer, MatResponseMode mmode,
                                       GaussPoint *gp,
                                       TimeStep *tStep);
    virtual void computePlaneStressStrain(FloatMatrix &answer, FloatMatrix damageTensor, FloatArray totalStrain, GaussPoint *gp,
                                          TimeStep *atTime);
    virtual void computePlaneStressSigmaZ(double &answer, FloatMatrix damageTensor, FloatArray reducedTotalStrainVector,
                                          double epsZ, GaussPoint *gp, TimeStep *atTime);

    /*    virtual void computeDamageTensor(FloatMatrix &answer, GaussPoint *gp,
     *                                   const FloatArray &totalStrain, double equivStrain,
     *                                   TimeStep *atTime);
     */

    virtual void computeSecantOperator(FloatMatrix &answer, FloatMatrix strainTensor,
                                       FloatMatrix damageTensor, GaussPoint *gp);

    double computeK(GaussPoint *gp);

    double computeKappa(FloatMatrix damageTensor);
};
} // end namespace oofem
#endif // anisodamagemodel_h
