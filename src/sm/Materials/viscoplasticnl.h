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

#ifndef viscoplasticnl_h

#include "Materials/viscoplastic.h"
#include "Materials/structuralnonlocalmaterialext.h"
#include "nonlocmatstiffinterface.h"
#include "cltypes.h"

///@name Input fields for ViscoPlasticNl
//@{
#define _IFT_ViscoPlasticNl_Name "viscoplasticnl"
#define _IFT_ViscoPlasticNl_averagingtype "averagingtype"
//@}

namespace oofem {
/**
 * Stress Gradient Plasticity with Viscoplastic Nonlocal material status.
 * @author Srinath Chakravarthy
 */
class ViscoPlasticNlStatus : public ViscoPlasticStatus, public StructuralNonlocalMaterialStatusExtensionInterface
{
protected:
    // STATE VARIABLE DECLARATION
    // Equivalent strain for avaraging
    double localCumPlasticStrainForAverage;
    /// Local Stress Gradient used for average value of stress gradient 
    double localXitaForAverage; 
    

public:
    ViscoPlasticNlStatus(int n, Domain * d, GaussPoint * g);
    virtual ~ViscoPlasticNlStatus();

    virtual void printOutputAt(FILE *file, TimeStep *tStep);

    // STATE VARIABLE
    // declare state variable access and modification methods
    double giveLocalCumPlasticStrainForAverage() { return localCumPlasticStrainForAverage; }
    
    double giveLocalXitaForAverage() { return localXitaForAverage; }
    
    
    void setLocalCumPlasticStrainForAverage(double ls) { localCumPlasticStrainForAverage = ls; }
    
    void setLocalXitaForAverage(double ls) { localXitaForAverage = ls; }

    // DEFINITION
    virtual const char *giveClassName() const { return "ViscoPlasticNlStatus"; }

    virtual void initTempStatus();

    virtual void updateYourself(TimeStep *tStep);
    virtual contextIOResultType saveContext(DataStream &stream, ContextMode mode, void *obj = NULL);
    virtual contextIOResultType restoreContext(DataStream &stream, ContextMode mode, void *obj = NULL);

    virtual Interface *giveInterface(InterfaceType);
};


/**
 * Stress Gradient Plasticity with Viscoplastic Nonlocal material.
 * @author Srinath Chakravarthy
 */
class ViscoPlasticNl : public ViscoPlastic, public StructuralNonlocalMaterialExtensionInterface
{
protected:
    double Rf;
    double exponent;
    int averType;

public:
    ViscoPlasticNl(int n, Domain * d);
    virtual ~ViscoPlasticNl();

    virtual const char *giveClassName() const { return "ViscoPlasticNl"; }
    virtual const char *giveInputRecordName() const { return _IFT_ViscoPlasticNl_Name; }

    virtual IRResultType initializeFrom(InputRecord *ir);
    virtual void giveInputRecord(DynamicInputRecord &input);

    virtual Interface *giveInterface(InterfaceType);
    
    void computeLocalXita(double &xita, GaussPoint *gp, TimeStep *tstep)
    {
      ViscoPlastic :: computeXita(xita, gp, tstep);
    }
    void computeLocalCumPlasticStrain(double &kappa, GaussPoint *gp, TimeStep *tStep)
    {
        ViscoPlastic :: computeCumPlastStrain(kappa, gp, tStep);
    }

    
    //virtual void givePlaneStrainStiffMtrx(FloatMatrix& answer, MatResponseMode, GaussPoint *gp,TimeStep *tStep);
    //virtual void give3dMaterialStiffnessMatrix(FloatMatrix& answer, MatResponseMode, GaussPoint *gp, TimeStep *tStep);


    virtual void giveRealStressVector_3d(FloatArray &answer,  GaussPoint *gp, const FloatArray &strainVector, TimeStep *tStep);
    
     /**
     * Computes the nonlocal Stress Gradient parameter
     * @param[out] xita Return param, containing the corresponding normalized stress gradient parameter
     * @param gp Integration point.
     * @param tStep Time step.
     */
    
    virtual void computeXita(double &xita, GaussPoint *gp, TimeStep *tStep);    
    


    virtual void updateBeforeNonlocAverage(const FloatArray &strainVector, GaussPoint *gp, TimeStep *tStep);

    virtual int hasBoundedSupport() { return 1; }

    virtual int packUnknowns(DataStream &buff, TimeStep *tStep, GaussPoint *ip);
    virtual int unpackAndUpdateUnknowns(DataStream &buff, TimeStep *tStep, GaussPoint *ip);
    virtual int estimatePackSize(DataStream &buff, GaussPoint *ip);

protected:
    // Creates the corresponding material status
    MaterialStatus *CreateStatus(GaussPoint *gp) const { return new ViscoPlasticNlStatus(1, ViscoPlastic :: domain, gp); }
};
} // end namespace oofem
#define viscoplasticnl_h
#endif
