/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/


namespace CGoGN
{

namespace Algo
{

namespace Multiresolution
{

template <typename PFP>
Map2MR_PM<PFP>::Map2MR_PM(MAP& map, VertexAttribute<VEC3>& position,
		Algo::Decimation::SelectorType s, Algo::Decimation::ApproximatorType a
		) :
		m_map(map), m_position(position)
{

	CGoGNout << "  creating approximator and predictor.." << CGoGNflush ;
	switch(a)
	{
	case Algo::Decimation::A_QEM : {
		m_approximators.push_back(new Algo::Decimation::Approximator_QEM<PFP>(m_map, m_position)) ;
		break ; }
	case Algo::Decimation::A_MidEdge : {
		m_approximators.push_back(new Algo::Decimation::Approximator_MidEdge<PFP>(m_map, m_position)) ;
		break ; }
	case Algo::Decimation::A_HalfCollapse : {
		Algo::Decimation::Predictor_HalfCollapse<PFP>* pred = new Algo::Decimation::Predictor_HalfCollapse<PFP>(m_map, m_position) ;
		m_predictors.push_back(pred) ;
		m_approximators.push_back(new Algo::Decimation::Approximator_HalfCollapse<PFP>(m_map, m_position, pred)) ;
		break ; }
	case Algo::Decimation::A_CornerCutting : {
		Algo::Decimation::Predictor_CornerCutting<PFP>* pred = new Algo::Decimation::Predictor_CornerCutting<PFP>(m_map, m_position) ;
		m_predictors.push_back(pred) ;
		m_approximators.push_back(new Algo::Decimation::Approximator_CornerCutting<PFP>(m_map, m_position, pred)) ;
		break ; }
	case Algo::Decimation::A_TangentPredict1 : {
		Algo::Decimation::Predictor_TangentPredict1<PFP>* pred = new Algo::Decimation::Predictor_TangentPredict1<PFP>(m_map, m_position) ;
		m_predictors.push_back(pred) ;
		m_approximators.push_back(new Algo::Decimation::Approximator_MidEdge<PFP>(m_map, m_position, pred)) ;
		break ; }
	case Algo::Decimation::A_TangentPredict2 : {
		Algo::Decimation::Predictor_TangentPredict2<PFP>* pred = new Algo::Decimation::Predictor_TangentPredict2<PFP>(m_map, m_position) ;
		m_predictors.push_back(pred) ;
		m_approximators.push_back(new Algo::Decimation::Approximator_MidEdge<PFP>(m_map, m_position, pred)) ;
		break ; }
	}
	CGoGNout << "..done" << CGoGNendl ;

	CGoGNout << "  creating selector.." << CGoGNflush ;
	switch(s)
	{
	case Algo::Decimation::S_MapOrder : {
		m_selector = new Algo::Decimation::EdgeSelector_MapOrder<PFP>(m_map, m_position, m_approximators, allDarts) ;
		break ; }
	case Algo::Decimation::S_Random : {
		m_selector = new Algo::Decimation::EdgeSelector_Random<PFP>(m_map, m_position, m_approximators, allDarts) ;
		break ; }
	case Algo::Decimation::S_EdgeLength : {
		m_selector = new Algo::Decimation::EdgeSelector_Length<PFP>(m_map, m_position, m_approximators, allDarts) ;
		break ; }
	case Algo::Decimation::S_QEM : {
		m_selector = new Algo::Decimation::EdgeSelector_QEM<PFP>(m_map, m_position, m_approximators, allDarts) ;
		break ; }
	case Algo::Decimation::S_MinDetail : {
		m_selector = new Algo::Decimation::EdgeSelector_MinDetail<PFP>(m_map, m_position, m_approximators, allDarts) ;
		break ; }
	case Algo::Decimation::S_Curvature : {
		m_selector = new Algo::Decimation::EdgeSelector_Curvature<PFP>(m_map, m_position, m_approximators, allDarts) ;
		break ; }
	}
	CGoGNout << "..done" << CGoGNendl ;

	m_initOk = true ;

	CGoGNout << "  initializing approximators.." << CGoGNflush ;
	for(typename std::vector<Algo::Decimation::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
	{
		if(! (*it)->init())
			m_initOk = false ;
		if((*it)->getApproximatedAttributeName() == "position")
			m_positionApproximator = reinterpret_cast<Algo::Decimation::Approximator<PFP, VEC3>*>(*it) ;
	}
	CGoGNout << "..done" << CGoGNendl ;

	CGoGNout << "  initializing predictors.." << CGoGNflush ;
	for(typename std::vector<Algo::Decimation::PredictorGen<PFP>*>::iterator it = m_predictors.begin(); it != m_predictors.end(); ++it)
		if(! (*it)->init())
			m_initOk = false ;
	CGoGNout << "..done" << CGoGNendl ;

	CGoGNout << "  initializing selector.." << CGoGNflush ;
	m_initOk = m_selector->init() ;
	CGoGNout << "..done" << CGoGNendl ;
}

template <typename PFP>
Map2MR_PM<PFP>::~Map2MR_PM()
{
	if(m_selector)
		delete m_selector ;
	for(typename std::vector<Algo::Decimation::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
		delete (*it) ;
	for(typename std::vector<Algo::Decimation::PredictorGen<PFP>*>::iterator it = m_predictors.begin(); it != m_predictors.end(); ++it)
		delete (*it) ;
}



template <typename PFP>
void Map2MR_PM<PFP>::addNewLevel(unsigned int percentWantedVertices)
{

	m_map.pushLevel() ;

	m_map.addFrontLevel();
	m_map.setCurrentLevel(0);
//	//Add the current level higher
//
//	unsigned int newLevel = m_map.getMaxLevel() + 1 ;
//	std::stringstream ss ;
//	ss << "MRdart_"<< newLevel ;
//
//	AttributeMultiVector<unsigned int>* newAttrib = m_map.addMRRelation(ss.str());
//	AttributeMultiVector<unsigned int>* newAttrib = m_mrattribs.addAttribute<unsigned int>(ss.str()) ;
//	m_mrDarts.push_back(newAttrib) ;
//	m_mrNbDarts.push_back(0) ;
//
//	//m_map.push_front(newAttrib);
//	if(m_mrDarts.size() > 1)
//	{
//		for(unsigned int i = newLevel; i > 0 ; --i)
//		{
//			AttributeMultiVector<unsigned int>* currentAttrib = m_mrDarts[i] ;
//			AttributeMultiVector<unsigned int>* prevAttrib = m_mrDarts[i - 1] ;	// copy the indices of
//			//m_mrattribs.copyAttribute(currentAttrib->getIndex(), prevAttrib->getIndex()) ;	// previous level into new level
//			m_mrattribs.swapAttributes(currentAttrib->getIndex(), prevAttrib->getIndex()) ;	// previous level into new level
//		}
//	}
//
//	m_map.popLevel() ;
//
//	m_map.setCurrentLevel(0);
}

template <typename PFP>
void Map2MR_PM<PFP>::createPM(unsigned int percentWantedVertices)
{
	//addNewLevel();
	m_map.pushLevel() ;

	m_map.addFrontLevel();
	m_map.setCurrentLevel(0) ;
//	std::cout << "level : " << m_map.getMaxLevel() << std::endl;
//
//	unsigned int nbVertices = m_map.template getNbOrbits<VERTEX>() ;
//	unsigned int nbWantedVertices = nbVertices * percentWantedVertices / 100 ;
//	CGoGNout << "  creating PM (" << nbVertices << " vertices).." << /* flush */ CGoGNendl ;
//
//	bool finished = false ;
//	Dart d ;
//	while(!finished)
//	{
//		if(!m_selector->nextEdge(d))
//			break ;
//
//		--nbVertices ;
//		Dart d2 = m_map.phi2(m_map.phi_1(d)) ;
//		Dart dd2 = m_map.phi2(m_map.phi_1(m_map.phi2(d))) ;
//
//		for(typename std::vector<Algo::Decimation::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
//		{
//			(*it)->approximate(d) ;					// compute approximated attributes with its associated detail
//			(*it)->saveApprox(d) ;
//		}
//
//		m_selector->updateBeforeCollapse(d) ;		// update selector
//
//		m_map.collapseEdge(d);
//
//		unsigned int newV = m_map.template embedNewCell<VERTEX>(d2) ;
//		unsigned int newE1 = m_map.template embedNewCell<EDGE>(d2) ;
//		unsigned int newE2 = m_map.template embedNewCell<EDGE>(dd2) ;
////		vs->setApproxV(newV) ;
////		vs->setApproxE1(newE1) ;
////		vs->setApproxE2(newE2) ;
//
//		for(typename std::vector<Algo::Decimation::ApproximatorGen<PFP>*>::iterator it = m_approximators.begin(); it != m_approximators.end(); ++it)
//			(*it)->affectApprox(d2);				// affect data to the resulting vertex
//
//		m_selector->updateAfterCollapse(d2, dd2) ;	// update selector
//
//		if(nbVertices <= nbWantedVertices)
//			finished = true ;
//	}
//	delete m_selector ;
//	m_selector = NULL ;
//
//	m_map.popLevel() ;
//	CGoGNout << "..done (" << nbVertices << " vertices)" << CGoGNendl ;
}

template <typename PFP>
void Map2MR_PM<PFP>::coarsen()
{
	assert(m_map.getCurrentLevel() > 0 || !"coarsen : called on level 0") ;

	m_map.decCurrentLevel() ;
}

template <typename PFP>
void Map2MR_PM<PFP>::refine()
{
	assert(m_map.getCurrentLevel() < m_map.getMaxLevel() || !"refine: called on max level") ;

	m_map.incCurrentLevel() ;
}

} // namespace Multiresolution

} // namespace Algo

} // namespace CGoGN