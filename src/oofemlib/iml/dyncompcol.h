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

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
/*        Compressed column sparse matrix  (O-based, Fortran)            */
/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/


#ifndef dyncompcol_h
#define dyncompcol_h

#include "sparsemtrx.h"
#include "intarray.h"

namespace oofem {
// alloc chunk for columns
#define DynCompCol_CHUNK 8
// turns on the stl set container, used to store column data, usually slower
//#define  DynCompCol_USE_STL_SETS


#ifdef DynCompCol_USE_STL_SETS
 #include <map>
#endif
/**
 * Implementation of sparse matrix stored in compressed column storage.
 * Designed to allow simple dynamic runtime grow of receiver.
 */
class OOFEM_EXPORT DynCompCol : public SparseMtrx
{
protected:

#ifndef DynCompCol_USE_STL_SETS
    FloatArray * * columns_; // data values per column
    IntArray **rowind_;   // row_ind per column
#else
    std :: vector< std :: map< int, double > > columns;
#endif

    int base_; // index base: offset of first element

public:
    /** Constructor. Before any operation an internal profile must be built.
     * @see builInternalStructure
     */
    DynCompCol(int n);
    /** Constructor. Before any operation an internal profile must be built.
     * @see builInternalStructure
     */
    DynCompCol();
    /// Copy constructor
    DynCompCol(const DynCompCol & S);
    /// Assignment operator
    DynCompCol &operator = ( const DynCompCol & C );
    /// Destructor
    virtual ~DynCompCol();

    // Overloaded methods:
    SparseMtrx *GiveCopy() const;
    virtual void times(const FloatArray &x, FloatArray &answer) const;
    virtual void timesT(const FloatArray &x, FloatArray &answer) const;
    virtual void times(double x);
    virtual int buildInternalStructure(EngngModel *, int, const UnknownNumberingScheme &);
    virtual int assemble(const IntArray &loc, const FloatMatrix &mat);
    virtual int assemble(const IntArray &rloc, const IntArray &cloc, const FloatMatrix &mat);
    virtual bool canBeFactorized() const { return false; }
    virtual void zero();
    virtual const char* giveClassName() const { return "DynCompCol"; }
    virtual SparseMtrxType  giveType() const { return SMT_DynCompCol; }
    virtual bool isAsymmetric() const { return true; }
    virtual double &at(int i, int j);
    virtual double at(int i, int j) const;
    virtual void printStatistics() const;

    /*******************************/
    /*  Access and info functions  */
    /*******************************/
#ifndef DynCompCol_USE_STL_SETS
    /// Returns row index for i-th  column
    const IntArray *row_ind(int i) const { return rowind_ [ i ]; }
    /// Returns column values
    const FloatArray *column(int i) const { return columns_ [ i ]; }
#else
    /// Returns column values
    std :: map< int, double > *column(int i) const { return columns [ i ]; }
#endif

protected:

    /***********************************/
    /*  General access function (slow) */
    /***********************************/
    /// Implements 0-based access
    double operator() (int i, int j) const;
    /// Implements 0-based access
    double &operator() (int i, int j);

#ifndef DynCompCol_USE_STL_SETS
    /// Returns the row index of given row at given column, else returns zero.
    int giveRowIndx(int col, int row) const;
    /// Insert row entry into column, preserving order of row indexes, returns the index of new row.
    int insertRowInColumn(int col, int row);
#endif

    void checkSizeTowards(IntArray &);
    void checkSizeTowards(const IntArray &rloc, const IntArray &cloc);
    void growTo(int);
};
} // end namespace oofem
#endif // dyncompcol_h
