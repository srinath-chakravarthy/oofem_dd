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

#include "load.h"
#include "verbose.h"
#include "timestep.h"
#include "function.h"
#include "dynamicinputrecord.h"

namespace oofem {
Load :: Load(int i, Domain *aDomain) :
    GeneralBoundaryCondition(i, aDomain), componentArray(), dofExcludeMask()
    // Constructor. Creates a load with number i, belonging to aDomain.
{
    timeFunction = 0;
}


FloatArray &Load :: giveComponentArray()
// Returns the array that contains the components of the receiver. If
// this array does not exist yet, forms it by reading its values in the
// data file.
{
    return componentArray;
}


void
Load :: computeComponentArrayAt(FloatArray &answer, TimeStep *tStep, ValueModeType mode)
// Returns an array, the load induced at tStep by the receiver.
{
    double factor;

    factor = this->giveTimeFunction()->evaluate(tStep, mode);
    answer  = this->giveComponentArray();
    answer.times(factor);

    if ( !isImposed(tStep) ) {
        answer.zero();
    }
}


IRResultType
Load :: initializeFrom(InputRecord *ir)
{
    IRResultType result;                // Required by IR_GIVE_FIELD macro

#  ifdef VERBOSE
    // VERBOSE_PRINT1 ("Instanciating load ",number)
#  endif

    IR_GIVE_FIELD(ir, componentArray, _IFT_Load_components);

    int size = componentArray.giveSize();
    dofExcludeMask.resize(size);
    dofExcludeMask.zero();
    IR_GIVE_OPTIONAL_FIELD(ir, dofExcludeMask, _IFT_Load_dofexcludemask);
    if ( dofExcludeMask.giveSize() != size ) {
        OOFEM_WARNING("dofExcludeMask and componentArray size mismatch");
        return IRRT_BAD_FORMAT;
    } else {
        for ( int i = 1; i <= size; i++ ) {
            if ( dofExcludeMask.at(i) ) {
                componentArray.at(i) = 0.0;
            }
        }
    }

    return GeneralBoundaryCondition :: initializeFrom(ir);
}


void Load :: giveInputRecord(DynamicInputRecord &input)
{
    GeneralBoundaryCondition :: giveInputRecord(input);
    input.setField(this->componentArray, _IFT_Load_components);
    if ( !this->dofExcludeMask.containsOnlyZeroes() ) {
        input.setField(this->dofExcludeMask, _IFT_Load_dofexcludemask);
    }
}


int
Load :: isDofExcluded(int indx)
{
    if ( ( indx > 0 ) && ( indx <= dofExcludeMask.giveSize() ) ) {
        return dofExcludeMask.at(indx);
    } else {
        OOFEM_ERROR("dof index out of range");
    }

    return 0;
}
} // end namespace oofem
