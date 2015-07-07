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

#ifndef rigidarmnode_h
#define rigidarmnode_h

#include "node.h"

///@name Input fields for RigidArmNode
//@{
#define _IFT_RigidArmNode_Name "rigidarmnode"
#define _IFT_RigidArmNode_master "master"
//@}

namespace oofem {
class FloatArray;
class IntArray;

/**
 * Class implementing node connected to other node (master) using rigid arm in finite element mesh.
 * Rigid arm node possess no degrees of freedom - all dofs are mapped to master dofs.
 * The Rigid arm node supports not only slave dofs mapped to master
 * but also some dofs can be primary dofs. Introduced masterDofMask allowing to
 * distinguish between primary and mapped (slave) dofs. The primary DOFs can have their own BCs, ICs.
 *
 * The introduction of rigid arm connected nodes allows to avoid very stiff elements used
 * for modeling the rigid-arm connection. The rigid arm node maps its dofs to master dofs
 * using simple transformations (small rotations are assumed). Therefore, the contribution
 * to rigid arm node are localized directly to master related equations.
 * The rigid arm node can not have its own boundary or initial conditions,
 * they are determined completely from master dof conditions.
 * The local coordinate system in slave is not supported in current implementation, the global cs applies.
 * On the other hand, rigid arm node can be loaded independently of master.
 * The transformation for DOFs and load is not orthogonal - the inverse transformation can
 * not be constructed by transposition. Because of time consuming inversion, methods
 * can generally compute both transformations for dofs as well as loads.
 */
class OOFEM_EXPORT RigidArmNode : public Node
{
protected:
    ///
    IntArray masterMask;
    /// Number of master DofManager (Node)
    int masterDofMngr;
    /// Pointer to master Node
    Node *masterNode;

private:
    void allocAuxArrays();
    void deallocAuxArrays();

public:
    /**
     * Constructor. Creates a rigid-arm node with number n, belonging to aDomain.
     * @param n Node number in domain aDomain.
     * @param aDomain Domain to which node belongs.
     */
    RigidArmNode(int n, Domain * aDomain);
    /// Destructor.
    virtual ~RigidArmNode(void) { }

    virtual IRResultType initializeFrom(InputRecord *ir);
    virtual void postInitialize();
    virtual void updateLocalNumbering(EntityRenumberingFunctor &f);
    virtual int checkConsistency();
    /**
     * Compute vector of master contribution coefficients - SUMA of contributions == 1.0
     */
    void computeMasterContribution(std::map< DofIDItem, IntArray > &masterDofID, 
                                   std::map< DofIDItem, FloatArray > &masterContribution);

    virtual const char *giveClassName() const { return "RigidArmNode"; }
    virtual const char *giveInputRecordName() const { return _IFT_RigidArmNode_Name; }
    virtual bool isDofTypeCompatible(dofType type) const { return ( type == DT_master || type == DT_slave ); }
};
} // end namespace oofem
#endif // rigidarmnode_h
