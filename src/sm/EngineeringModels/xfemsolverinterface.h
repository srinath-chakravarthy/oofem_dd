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

#ifndef XFEMSOLVERINTERFACE_H_
#define XFEMSOLVERINTERFACE_H_

namespace oofem {

class TimeStep;
class StructuralEngngModel;
class StaticStructural;
class FloatArray;

/**
 * Provides extra solver functionality needed for XFEM.
 * @author Erik Svenning
 */
class XfemSolverInterface {
public:
    XfemSolverInterface();
    virtual ~XfemSolverInterface();

    void propagateXfemInterfaces(TimeStep *tStep, StructuralEngngModel &ioEngngModel, bool iRecomputeStepAfterCrackProp);
    void mapVariables(TimeStep *tStep, StructuralEngngModel &ioEngngModel);

#if 0
    void mapVariables(TimeStep *tStep, StaticStructural &ioEngngModel);
#endif

    void xfemUpdatePrimaryField(StructuralEngngModel &ioEngngModel, TimeStep *tStep, const FloatArray &iNewSol) { }
    void xfemUpdatePrimaryField(StaticStructural &ioEngngModel, TimeStep *tStep, const FloatArray &iNewSol);

protected:
    bool mNeedsVariableMapping;
};

} /* namespace oofem */

#endif /* XFEMSOLVERINTERFACE_H_ */
