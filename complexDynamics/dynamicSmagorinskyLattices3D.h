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

/** \file
 * Descriptor for nearest-neighbor 3D lattices. In principle, thanks
 * to static genericity of the code, definition of the lattice and
 * implementation of the dynamics are independent. Of course, there
 * remain a few dependencies. For example, a dynamics which uses higher-
 * order moments such as heat flux needs a lattice with an extended
 * neighborhood. Another example is the D3Q13 lattice, which works only
 * with its very own dynamics.
 *
 * Lattice descriptors for dynamic Smagorinsky models -- header file
 */
#ifndef DYNAMIC_SMAGORINSKY_LATTICES_3D_H
#define DYNAMIC_SMAGORINSKY_LATTICES_3D_H

#include "core/globalDefs.h"
#include "latticeBoltzmann/nearestNeighborLattices3D.h"

namespace plb {

namespace descriptors {

    struct ExternalOmegaDescriptor3d {
        static const int numScalars    = 1;
        static const int numSpecies    = 1;
        static const int omegaBeginsAt = 0;
        static const int sizeOfOmega   = 1;
    };

    struct ExternalOmegaBase3d {
        typedef ExternalOmegaDescriptor3d ExternalField;
    };

    struct ExternalOmegaAndForceDescriptor3d {
        static const int numScalars    = 4;
        static const int numSpecies    = 2;
        static const int omegaBeginsAt = 0;
        static const int sizeOfOmega   = 1;
        static const int forceBeginsAt = 1;
        static const int sizeOfForce   = 3;
    };

    struct ExternalOmegaAndForceBase3d {
        typedef ExternalOmegaAndForceDescriptor3d ExternalField;
    };

    template <typename T> struct ExternalOmegaD3Q19Descriptor
        : public D3Q19DescriptorBase<T>, public ExternalOmegaBase3d
    {
        static const char name[];
    };

    template <typename T> struct ForcedExternalOmegaD3Q19Descriptor
        : public D3Q19DescriptorBase<T>, public ExternalOmegaAndForceBase3d
    {
        static const char name[];
    };

    template<typename T>
    const char ExternalOmegaD3Q19Descriptor<T>::name[] = "ExternalOmegaD3Q19";

    template<typename T>
    const char ForcedExternalOmegaD3Q19Descriptor<T>::name[] = "ExternalOmegaAndForceD3Q19";


    template <typename T> struct ExternalOmegaD3Q15Descriptor
        : public D3Q15DescriptorBase<T>, public ExternalOmegaBase3d
    {
        static const char name[];
    };

    template <typename T> struct ForcedExternalOmegaD3Q15Descriptor
        : public D3Q15DescriptorBase<T>, public ExternalOmegaAndForceBase3d
    {
        static const char name[];
    };

    template<typename T>
    const char ExternalOmegaD3Q15Descriptor<T>::name[] = "ExternalOmegaD3Q15";

    template<typename T>
    const char ForcedExternalOmegaD3Q15Descriptor<T>::name[] = "ExternalOmegaAndForceD3Q15";


    template <typename T> struct ExternalOmegaD3Q27Descriptor
        : public D3Q27DescriptorBase<T>, public ExternalOmegaBase3d
    {
        static const char name[];
    };

    template <typename T> struct ForcedExternalOmegaD3Q27Descriptor
        : public D3Q27DescriptorBase<T>, public ExternalOmegaAndForceBase3d
    {
        static const char name[];
    };

    template<typename T>
    const char ExternalOmegaD3Q27Descriptor<T>::name[] = "ExternalOmegaD3Q27";

    template<typename T>
    const char ForcedExternalOmegaD3Q27Descriptor<T>::name[] = "ExternalOmegaAndForceD3Q27";

}  // namespace descriptors

}  // namespace plb

#endif  // DYNAMIC_SMAGORINSKY_LATTICES_3D_H
