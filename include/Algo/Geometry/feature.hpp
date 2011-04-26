/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009, IGG Team, LSIIT, University of Strasbourg                *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: https://iggservis.u-strasbg.fr/CGoGN/                              *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#include "Geometry/basic.h"
#include "Algo/Geometry/normal.h"

namespace CGoGN
{

namespace Algo
{

namespace Geometry
{

template <typename PFP>
void featureEdgeDetection(typename PFP::MAP& map, typename PFP::TVEC3& position, DartMarker& feature)
{
	typedef typename PFP::VEC3 VEC3 ;
	typedef typename PFP::REAL REAL ;

	feature.unmarkAll() ;

	AttributeHandler<VEC3> fNormal = map.template addAttribute<VEC3>(FACE_ORBIT, "fNormal") ;
	Algo::Geometry::computeNormalFaces<PFP>(map, position, fNormal) ;

	DartMarker m(map) ;
	for(Dart d = map.begin(); d != map.end(); map.next(d))
	{
		if(!m.isMarked(d))
		{
			m.markOrbit(EDGE_ORBIT, d) ;
			if(Geom::angle(fNormal[d], fNormal[map.phi2(d)]) > M_PI / REAL(6))
				feature.markOrbit(EDGE_ORBIT, d) ;
		}
	}

	map.template removeAttribute<VEC3>(fNormal) ;
}

} // namespace Geometry

} // namespace Algo

} // namespace CGoGN