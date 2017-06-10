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

/* Main author: Orestis Malaspinas.
 */

/** \file
 * Neumann boundary conditions -- generic implementation.
 */
#ifndef ADIABATIC_BOUNDARY_PROCESSOR_3D_HH
#define ADIABATIC_BOUNDARY_PROCESSOR_3D_HH

#include "complexDynamics/adiabaticBoundaryProcessor3D.h"
#include "finiteDifference/fdStencils1D.h"

namespace plb {

template<typename T, template<typename U> class Descriptor, int direction, int orientation> 
void FlatAdiabaticBoundaryFunctional3D<T,Descriptor,direction,orientation>::process (
        Box3D domain, BlockLattice3D<T,Descriptor>& lattice )
{
    for (plint iX=domain.x0; iX<=domain.x1; ++iX) 
    {
        plint iX_prev = iX + ( (direction==0) ? (-orientation) : 0 );
        for (plint iY=domain.y0; iY<=domain.y1; ++iY) 
        {
            plint iY_prev = iY + ( (direction==1) ? (-orientation) : 0 );
            for (plint iZ=domain.z0; iZ<=domain.z1; ++iZ) 
            {
                plint iZ_prev = iZ + ( (direction==2) ? (-orientation) : 0 );
                
                T temperature_1 = lattice.get(iX_prev,iY_prev,iZ_prev).computeDensity();
                lattice.get(iX,iY,iZ).defineDensity(temperature_1);
            }
        }
    }
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation> 
FlatAdiabaticBoundaryFunctional3D<T,Descriptor,direction,orientation>*
    FlatAdiabaticBoundaryFunctional3D<T,Descriptor,direction,orientation>::clone() const
{
    return new FlatAdiabaticBoundaryFunctional3D<T,Descriptor,direction,orientation>(*this);
}

template<typename T, template<typename U> class Descriptor, int direction, int orientation> 
void FlatAdiabaticBoundaryFunctional3D<T,Descriptor,direction,orientation>::getTypeOfModification (
        std::vector<modif::ModifT>& modified ) const
{
    modified[0] = modif::dynamicVariables;
}

}  // namespace plb

#endif  // NEUMANN_CONDITION_3D_HH

