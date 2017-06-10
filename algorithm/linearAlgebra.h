/* This file is part of the Palabos library.
 *
 * Copyright (C) 2011-2015 FlowKit Sarl
 * Route d'Oron 2
 * 1010 Lausanne, Switzerland
 * E-mail contact: contact@flowkit.com
 *
 * The most recent release of Palabos can be downloaded at 
 * <http://www.palabos.org/>
 *
 * The library Palabos is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * The library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LINEAR_ALGEBRA_H
#define LINEAR_ALGEBRA_H

#include "core/globalDefs.h"
#include "core/array.h"

namespace plb {

// Compute two 3D unit vectors v2Unit and v3Unit that are orthogonal to a given unit
//   vector v1Unit and to each other.
//   ATTENTION: it is a prerequisite that the input vector v1Unit is unitary. If you
//   have a non-unitary vector v1, you should provide v1/norm(v1) as argument.
template<typename T>
void gramSchmidt(Array<T,3> const& v1Unit, Array<T,3>& v2Unit, Array<T,3>& v3Unit);

// Computation of eigen-vectors and eigen-values for symmetric 3x3 matrices.
/**Symmetric matrix A => eigenvectors in columns of V, corresponding
   eigenvalues in d.
   Public domain, copied from the public domain Java library JAMA.
   Original code at http://barnesc.blogspot.com/2007/02/eigenvectors-of-3x3-symmetric-matrix.html
**/
//\TODO:
// (1) Switch the Array<Array<T,3>,3> type to SymmetricTensor, as used in Palabos.
// (2) Change the "eps = pow(2.0,-52.0)" line in the code to something that's type-specific.
template<typename T>
void eigenDecomposition(Array<Array<T,3>,3> const& A, Array<Array<T,3>,3>& V, Array<T,3>& d);

} // namespace plb

#endif  // LINEAR_ALGEBRA_H

