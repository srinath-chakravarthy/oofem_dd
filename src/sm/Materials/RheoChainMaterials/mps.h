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

//   ***********************************************************************************
//   *** CLASS Material Model based no Microprestress Solidification Theory & Status ***
//   ***********************************************************************************

#ifndef mps_h
#define mps_h

#include "Materials/RheoChainMaterials/kelvinChSolM.h"

///@name Input fields for MPSMaterial
//@{
#define _IFT_MPSMaterial_Name "mps"
#define _IFT_MPSMaterial_talpha "talpha"
#define _IFT_MPSMaterial_mode "mode"
#define _IFT_MPSMaterial_coupledanalysistype "coupledanalysistype"
#define _IFT_MPSMaterial_fc "fc"
#define _IFT_MPSMaterial_cc "cc"
#define _IFT_MPSMaterial_wc "w/c"
#define _IFT_MPSMaterial_ac "a/c"
#define _IFT_MPSMaterial_q1 "q1"
#define _IFT_MPSMaterial_q2 "q2"
#define _IFT_MPSMaterial_q3 "q3"
#define _IFT_MPSMaterial_q4 "q4"
#define _IFT_MPSMaterial_lambda0 "lambda0"
#define _IFT_MPSMaterial_t0 "t0"
#define _IFT_MPSMaterial_ksh "ksh"
#define _IFT_MPSMaterial_wh "w_h"
#define _IFT_MPSMaterial_ncoeff "ncoeff"
#define _IFT_MPSMaterial_a "a"
#define _IFT_MPSMaterial_qetor "qetor"
#define _IFT_MPSMaterial_qrtor "qrtor"
#define _IFT_MPSMaterial_qstor "qstor"
#define _IFT_MPSMaterial_alphae "alphae"
#define _IFT_MPSMaterial_alphar "alphar"
#define _IFT_MPSMaterial_alphas "alphas"
#define _IFT_MPSMaterial_mus "mus"
#define _IFT_MPSMaterial_kappat "kappat"
#define _IFT_MPSMaterial_ct "ct"
#define _IFT_MPSMaterial_stiffnessfactor "stiffnessfactor"
//@}

namespace oofem {
/**
 * This class implements associated Material Status to MPSMaterial,
 * which corresponds to a model for humidity- and temperature-dependent
 * creep of concrete according to the microprestress-solidification theory.
 * At room temperature and 100% relative humidity, it reduces to model B3
 * for basic creep of concrete.
 */
class MPSMaterialStatus : public KelvinChainSolidMaterialStatus
{
protected:
    /// Values of humidity and temperature in a particular GP and their increment
    double hum;
    double hum_increment;
    double T;
    double T_increment;
    double T_max;
    /// Hidden variable - equivalent time: necessary to compute solidified volume
    double equivalentTime;
    double equivalentTimeTemp;
    double flowTermViscosity;
    double flowTermViscosityTemp;

public:
    MPSMaterialStatus(int n, Domain * d, GaussPoint * g, int nunits);
    virtual ~MPSMaterialStatus() { }

    virtual void updateYourself(TimeStep *tStep);

    virtual contextIOResultType saveContext(DataStream &stream, ContextMode mode, void *obj = NULL);
    virtual contextIOResultType restoreContext(DataStream &stream, ContextMode mode, void *obj = NULL);

    /// Returns relative humidity
    double giveHum() { return hum; }
    /// Stores relative humidity
    void setHum(double src) { hum = src; }
    /// Returns relative humidity increment
    double giveHumIncrement() { return hum_increment; }
    /// Stores relative humidity increment
    void setHumIncrement(double src) { hum_increment = src; }

    /// Returns temperature
    double giveT() { return T; }
    /// Stores temperature
    void setT(double src) { T = src; }

    /// Returns temperature increment
    double giveTIncrement() { return T_increment; }
    /// Stores temperature increment
    void setTIncrement(double src) { T_increment = src; }

    /// Returns previously maximum reached temperature
    double giveTmax() { return T_max; }
    /// Stores maximum reached temperature
    void setTmax(double src) { T_max = src; }

    /// Returns equivalent time
    double giveEquivalentTime() { return equivalentTime; }
    /// Stores equivalent time
    void setEquivalentTime(double src) { equivalentTimeTemp = src; }

    /// Returns viscosity of the flow term (associated with q4 and microprestress evolution)
    double giveFlowTermViscosity() { return flowTermViscosity; }
    double giveFlowTermViscosityTemp() { return flowTermViscosityTemp; }
    void setFlowTermViscosityTemp(double src) { flowTermViscosityTemp = src; }

    // definition
    virtual const char *giveClassName() const { return "MPSMaterialStatus"; }
};


/**
 * This class implements the extended B3 model for concrete creep and shrinkage
 * based on the microprestress-solidification theory.
 * The implementation exploits a solidifying Kelvin chain.
 * Creep is affected by variable temperature and humidity.
 * Comparing to other material models for concrete creep MPS material model is
 * unit independent (except parameters corresponding to concrete strength
 * and composition: fc, c, wc, ac ).
 * In the input record is is necessary to specify time factor equal to 1!!!
 */
class MPSMaterial : public KelvinChainSolidMaterial
{
protected:

    /// thermal dilatation coeff.
    double talpha;
    /// age when temperature or humidity starts to change
    double t0;
    /// compliances of the B3 model
    double q1, q2, q3, q4;
    /// constant equal to one day in time units of analysis (eg. 86400 if the analysis runs in seconds)
    double lambda0;


    enum coupledAnalysisType { Basic, MPS } CoupledAnalysis;

    double EspringVal; // elastic modulus of the aging spring (first member of Kelvin chain if retardation spectrum is used)

    /// additional parameters for sorption isotherm (used to compute relative humidity from water content)
    double w_h, n, a; //constant (obtained from experiments) A [Pedersen, 1990]

    // MPS theory parameters
    /// proportionality parameter between change of humidity and shrinkage
    double kSh;
    /// fluidity parameter used in viscosity evolution equation
    double muS;
    /// kappaT replaces ln(h) on RHS of the differential equation describing evolution of MPS
    double kappaT;
    /// parameter reducing creep effects of thermal cycling
    double ct;
    /// reference room temperature for MPS algorithm [K]
    double roomTemperature;
    /// activation energies
    double QEtoR, QRtoR, QStoR; //[K]
    /// parameters that control the effect of humidity on rates of hydration, creep and microprestress relaxation
    double alphaE, alphaR, alphaS; //[-]

public:
    MPSMaterial(int n, Domain * d) : KelvinChainSolidMaterial(n, d) { }
    virtual ~MPSMaterial() { }

    virtual const char *giveInputRecordName() const { return _IFT_MPSMaterial_Name; }
    virtual const char *giveClassName() const { return "MPSMaterial"; }

    virtual IRResultType initializeFrom(InputRecord *ir);

    virtual void giveRealStressVector(FloatArray &answer, GaussPoint *gp, const FloatArray &reducedStrain, TimeStep *tStep);
    //virtual void updateYourself(GaussPoint *gp, TimeStep *tStep);

    virtual void giveThermalDilatationVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep);

    virtual void giveShrinkageStrainVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep, ValueModeType mode);

    virtual MaterialStatus *CreateStatus(GaussPoint *gp) const;

protected:
    void predictParametersFrom(double, double, double, double, double);

    virtual void computeCharTimes();

    /// Evaluation of characteristic moduli of the non-aging Kelvin chain
    virtual void computeCharCoefficients(FloatArray &answer, double);

    virtual double giveEModulus(GaussPoint *gp, TimeStep *tStep);

    virtual double computeSolidifiedVolume(GaussPoint *gp, TimeStep *tStep);

    virtual double computeBetaMu(GaussPoint *gp, TimeStep *tStep, int Mu);
    virtual double computeLambdaMu(GaussPoint *gp, TimeStep *tStep, int Mu);

    /// Evaluation of the flow term viscosity
    double computeFlowTermViscosity(GaussPoint *gp, TimeStep *tStep);

    /// Returns initial value of the flow term viscosity
    double giveInitViscosity(TimeStep *tStep);

    virtual void  giveEigenStrainVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep, ValueModeType mode);

    virtual int hasIncrementalShrinkageFormulation() { return 1; }


    /// Evaluation of the shrinkageStrainVector - shrinkage is fully dependent on humidity rate in given GP
    void computePointShrinkageStrainVector(FloatArray &answer, GaussPoint *gp, TimeStep *tStep);


    double inverse_sorption_isotherm(double w);

    /// Gives value of humidity at given GP and timestep
    /// option = 0 ... beginning of the time step
    /// option = 1 ... end of the time step
    /// option = 2 ... average values
    /// option = 3 ... incremental values
    double giveHumidity(GaussPoint *gp, TimeStep *tStep, int option);

    /// Gives value of temperature at given GP and timestep
    /// option = 0 ... beginning of the time step
    /// option = 1 ... end of the time step
    /// option = 2 ... average values
    /// option = 3 ... incremental values
    double giveTemperature(GaussPoint *gp, TimeStep *tStep, int option);

    /// Evaluation of the factor transforming real time to reduced time (effect on the flow term)
    /// option = 0 ... beginning of the time step
    /// option = 1 ... end of the time step
    /// option = 2 ... average value
    double computePsiR(GaussPoint *gp, TimeStep *tStep, int option);

    /// Evaluation of the factor transforming real time to reduced time (effect on the evolution of microprestress)
    double computePsiS(GaussPoint *gp, TimeStep *tStep);

    /// Evaluation of the factor transforming real time to equivalent time (effect on the solidified volume)
    double computePsiE(GaussPoint *gp, TimeStep *tStep);

    /// Computes equivalent time at given time step and GP.
    /// If option == 0, equivalentTime is evaluated in the middle of the time step (to determine solidified ratio).
    /// If option == 1, equivalentTime is evaluated at the end of the time step. (for updating).
    double computeEquivalentTime(GaussPoint *gp, TimeStep *tStep, int option);

    friend class RankineMPSmat;
};
} // end namespace oofem
#endif // mps_h
