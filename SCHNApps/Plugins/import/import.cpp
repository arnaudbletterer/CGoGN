#include "import.h"

#include "system.h"
#include "mapHandler.h"

#include "Algo/Import/import.h"

#include <QFileDialog>
#include <QFileInfo>

bool ImportPlugin::enable()
{
	importAction = new QAction("import", this);
	addMenuAction("Import;Import Surface", importAction);
	connect(importAction, SIGNAL(triggered()), this, SLOT(cb_import()));
	return true;
}

void ImportPlugin::disable()
{
}

void ImportPlugin::cb_import()
{
	QString fileName = QFileDialog::getOpenFileName(m_window, "Import file", m_window->getAppPath(), "Mesh Files (*.ply *.off)");
	QFileInfo fi(fileName);

	if(fi.exists())
	{
		MAP* m = new MAP();
		MapHandler<PFP>* h = new MapHandler<PFP>(fi.baseName(), m_window, m);

		std::vector<std::string> attrNames ;
		Algo::Import::importMesh<PFP>(*m, fileName.toUtf8().constData(), attrNames);

		// get vertex position attribute
		VertexAttribute<VEC3> position = m->getAttribute<VEC3, CGoGN::VERTEX>(attrNames[0]);

		// create VBO for vertex position attribute
		h->createVBO(position);

		// compute vertex normal attribute
		VertexAttribute<VEC3> normal = m->getAttribute<VEC3, CGoGN::VERTEX>("normal");
		if(!normal.isValid())
			normal = m->addAttribute<VEC3, CGoGN::VERTEX>("normal");
		Algo::Geometry::computeNormalVertices<PFP>(*m, position, normal);

		// create VBO for vertex normal attribute
		h->createVBO(normal);

		// compute map bounding box
		h->updateBB(position);

		// compute primitive connectivity VBOs
		h->updatePrimitives(CGoGN::Algo::Render::GL2::POINTS, CGoGN::allDarts);
		h->updatePrimitives(CGoGN::Algo::Render::GL2::LINES, CGoGN::allDarts);
		h->updatePrimitives(CGoGN::Algo::Render::GL2::TRIANGLES, CGoGN::allDarts);

		m_window->addMap(h);
	}
}

/**
 * If we want to compile this plugin in debug mode,
 * we also define a DEBUG macro at the compilation
 */
#ifndef DEBUG
// essential Qt function:
// arguments are
//  - the compiled name of the plugin
//  - the main class of our plugin
Q_EXPORT_PLUGIN2(ImportPlugin, ImportPlugin)
#else
Q_EXPORT_PLUGIN2(ImportPluginD, ImportPlugin)
#endif