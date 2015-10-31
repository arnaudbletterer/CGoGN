#include "mapHandler.h"

namespace CGoGN
{

namespace SCHNApps
{

MapHandlerGen::MapHandlerGen(const QString& name, SCHNApps* s, GenericMap* map) :
	m_showBB(true),
	m_name(name),
	m_schnapps(s),
	m_map(map),
	m_frame(NULL),
	m_bbVertexAttribute(NULL),
	m_bbDrawer(NULL),
	m_render(NULL),
	m_topoRender(NULL)
{
	m_frame = new qglviewer::ManipulatedFrame();
	connect(m_frame, SIGNAL(manipulated()), this, SLOT(frameModified()));
}

MapHandlerGen::~MapHandlerGen()
{
	if(m_frame)
		delete m_frame;
	if(m_bbDrawer)
		delete m_bbDrawer;
	if(m_render)
		delete m_render;
	foreach(CGoGN::Utils::VBO* vbo, m_vbo)
		delete vbo;

	deleteTopoRender();
}

/*********************************************************
 * MANAGE ATTRIBUTES
 *********************************************************/

void MapHandlerGen::notifyAttributeModification(const AttributeHandlerGen& attr, bool update)
{
	QString nameAttr = QString::fromStdString(attr.name());

	if (m_vbo.contains(nameAttr))
		m_vbo[nameAttr]->updateData(attr);

	if (m_bbVertexAttribute && m_bbVertexAttribute->getName() == attr.name())
		updateBB();

	DEBUG_EMIT("attributeModified");
	emit(attributeModified(attr.getOrbit(), nameAttr));

    if(update)
    {
        foreach(View* view, l_views)
            view->updateGL();
    }
}

void MapHandlerGen::notifyConnectivityModification(bool update)
{
	if (m_render)
	{
		m_render->setPrimitiveDirty(Algo::Render::GL2::POINTS);
		m_render->setPrimitiveDirty(Algo::Render::GL2::LINES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::TRIANGLES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::BOUNDARY);
	}

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		foreach (CellSelectorGen* cs, m_cellSelectors[orbit])
			cs->rebuild();
	}

	DEBUG_EMIT("connectivityModified");
	emit(connectivityModified());

    if(update)
    {
        foreach(View* view, l_views)
            view->updateGL();
    }
}

void MapHandlerGen::clear(bool removeAttrib)
{
	if (m_render)
	{
		m_render->setPrimitiveDirty(Algo::Render::GL2::POINTS);
		m_render->setPrimitiveDirty(Algo::Render::GL2::LINES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::TRIANGLES);
		m_render->setPrimitiveDirty(Algo::Render::GL2::BOUNDARY);
	}

	for(unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		foreach (CellSelectorGen* cs, m_cellSelectors[orbit])
			cs->rebuild();
	}

	std::vector<std::string> attrs[NB_ORBITS];

	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		if(m_map->isOrbitEmbedded(orbit))
		{
			AttributeContainer& cont = m_map->getAttributeContainer(orbit);
			cont.getAttributesNames(attrs[orbit]);
		}
	}

	m_map->clear(removeAttrib);

	for (unsigned int orbit = 0; orbit < NB_ORBITS; ++orbit)
	{
		for (unsigned int i = 0; i < attrs[orbit].size(); ++i)
		{
			QString name = QString::fromStdString(attrs[orbit][i]);
			if (removeAttrib)
			{
				if (orbit == VERTEX)
					deleteVBO(name);
				DEBUG_EMIT("attributeRemoved");
				emit(attributeRemoved(orbit, name));
			}
			else
			{
				if (orbit == VERTEX)
					updateVBO(name);
				DEBUG_EMIT("attributeModified");
				emit(attributeModified(orbit, name));
			}
		}
	}

	DEBUG_EMIT("connectivityModified");
	emit(connectivityModified());
}

/*********************************************************
 * MANAGE VBOs
 *********************************************************/

Utils::VBO* MapHandlerGen::createVBO(const AttributeMultiVectorGen* attr)
{
	if(attr)
	{
		// RECORDING
		QTextStream* rec = m_schnapps->pythonStreamRecorder();
		if (rec)
			*rec << this->getName() << ".createVBO(\"" << QString(attr->getName().c_str()) << "\");" << endl;

		QString name = QString::fromStdString(attr->getName());
		Utils::VBO* vbo = getVBO(name);
		if(!vbo)
		{
			vbo = new Utils::VBO(attr->getName());
			vbo->updateData(attr);
			m_vbo.insert(name, vbo);
			DEBUG_EMIT("vboAdded");
			emit(vboAdded(vbo));
		}
		else
			vbo->updateData(attr);
		return vbo;
	}
	else
		return NULL;
}

Utils::VBO* MapHandlerGen::createVBO(const AttributeHandlerGen& attr)
{
	return createVBO(attr.getDataVectorGen());
}

Utils::VBO* MapHandlerGen::createVBO(const QString& name)
{
	return createVBO(m_map->getAttributeVectorGen(VERTEX, name.toUtf8().constData()));
}

void MapHandlerGen::updateVBO(const AttributeMultiVectorGen* attr)
{
	if(attr)
	{
		Utils::VBO* vbo = getVBO(QString::fromStdString(attr->getName()));
		if(vbo)
			vbo->updateData(attr);
	}
}

void MapHandlerGen::updateVBO(const AttributeHandlerGen& attr)
{
	return updateVBO(attr.getDataVectorGen());
}

void MapHandlerGen::updateVBO(const QString& name)
{
	updateVBO(m_map->getAttributeVectorGen(VERTEX, name.toUtf8().constData()));
}

Utils::VBO* MapHandlerGen::getVBO(const QString& name) const
{
	if (m_vbo.contains(name))
		return m_vbo[name];
	else
		return NULL;
}

void MapHandlerGen::deleteVBO(const QString& name)
{
	if (m_vbo.contains(name))
	{
		Utils::VBO* vbo = m_vbo[name];
		m_vbo.remove(name);
		DEBUG_EMIT("vboRemoved");
		emit(vboRemoved(vbo));
		delete vbo;
	}
}

/*********************************************************
 * MANAGE CELL SELECTORS
 *********************************************************/

void MapHandlerGen::removeCellSelector(unsigned int orbit, const QString& name)
{
	CellSelectorGen* cs = getCellSelector(orbit, name);
	if (cs)
	{
		m_cellSelectors[orbit].remove(name);
		DEBUG_EMIT("cellSelectorRemoved");
		emit(cellSelectorRemoved(orbit, name));

		disconnect(cs, SIGNAL(selectedCellsChanged()), this, SIGNAL(selectedCellsChanged()));

		delete cs;
	}
}

CellSelectorGen* MapHandlerGen::getCellSelector(unsigned int orbit, const QString& name) const
{
	if (m_cellSelectors[orbit].contains(name))
		return m_cellSelectors[orbit][name];
	else
		return NULL;
}

void MapHandlerGen::selectedCellsChanged()
{
	CellSelectorGen* cs = static_cast<CellSelectorGen*>(QObject::sender());
	DEBUG_EMIT("selectedCellsChanged");
	emit(selectedCellsChanged(cs));
}

void MapHandlerGen::updateMutuallyExclusiveSelectors(unsigned int orbit)
{
	QList<CellSelectorGen*> mex;
	foreach(CellSelectorGen* cs, m_cellSelectors[orbit])
	{
		if(cs->isMutuallyExclusive())
			mex.append(cs);
	}
	foreach(CellSelectorGen* cs, m_cellSelectors[orbit])
		cs->setMutuallyExclusiveSet(mex);
}

/*********************************************************
 * MANAGE LINKED VIEWS
 *********************************************************/

void MapHandlerGen::linkView(View* view)
{
	if(view && !l_views.contains(view))
		l_views.push_back(view);
}

void MapHandlerGen::unlinkView(View* view)
{
	l_views.removeOne(view);
}


void MapHandlerGen::deleteTopoRender()
{
	if (m_topoRender)
		delete m_topoRender;
}


} // namespace SCHNApps

} // namespace CGoGN
