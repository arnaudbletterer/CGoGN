/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2011, IGG Team, LSIIT, University of Strasbourg           *
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
* Web site: http://cgogn.u-strasbg.fr/                                         *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

#ifndef _UTIL_SVG_
#define _UTIL_SVG_

#include <vector>
#include <fstream>
#include <sstream>
#include <string>

#include "Geometry/vector_gen.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_precision.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_projection.hpp"
#include "glm/gtc/matrix_transform.hpp"


namespace CGoGN
{

namespace Utils
{

namespace SVG
{


struct DepthSort
{
	unsigned int obj;
	unsigned int id;
	float depth;
	DepthSort(unsigned int o, unsigned int i, float d):
		obj(o),id(i),depth(d) {}
	bool operator<(const DepthSort& ds) const { return depth > ds.depth; /* inverse depth sortin*/}

};


class SvgObj
{
protected:
	std::vector<Geom::Vec3f> m_vertices;
	std::vector<Geom::Vec3f> m_colors;
	std::vector<std::string> m_strings;
	std::vector<Geom::Vec3f> m_vertices3D;
	Geom::Vec3f m_color;
	float m_width;
public:
	virtual ~SvgObj() {}

	void addVertex(const Geom::Vec3f& v);

	void addVertex3D(const Geom::Vec3f& v);

	void addVertex(const Geom::Vec3f& v, const Geom::Vec3f& C);

	void addVertex3D(const Geom::Vec3f& v, const Geom::Vec3f& C);

	void addString(const Geom::Vec3f& v, const std::string& str);

	void addString(const Geom::Vec3f& v, const std::string& str, const Geom::Vec3f& C);

	void setColor(const Geom::Vec3f& c);

	void setWidth(float w);

	void close();

	virtual void save(std::ofstream& out) const = 0;

	virtual void saveOne(std::ofstream& out, unsigned int i, unsigned int bbl = 0) const = 0;

	unsigned int nbv() const;

	virtual unsigned int nbPrimtives() const = 0;

	virtual void fillDS(std::vector<DepthSort>& vds, unsigned int idObj) const = 0;

	const Geom::Vec3f& P(unsigned int i) const;

	Geom::Vec3f normal();

	const std::vector<Geom::Vec3f>& vertices() const;

};

class SvgPoints: public SvgObj
{
public:
	void save(std::ofstream& out) const;
	void saveOne(std::ofstream& out, unsigned int i, unsigned int bbl = 0) const;
	unsigned int nbPrimtives() const;
	void fillDS(std::vector<DepthSort>& vds, unsigned int idObj) const;
};

class SvgLines: public SvgObj
{
public:
	void save(std::ofstream& out) const;
	void saveOne(std::ofstream& out, unsigned int i, unsigned int bbl = 0) const;
	unsigned int nbPrimtives() const;
	void fillDS(std::vector<DepthSort>& vds, unsigned int idObj) const;
};


class SvgStrings: public SvgObj
{
protected:
	float m_sf;
public:
	SvgStrings(float scalefactor = 1.0f) : m_sf(scalefactor) {}
	void save(std::ofstream& out) const;
	void saveOne(std::ofstream& out, unsigned int i, unsigned int bbl = 0) const;
	unsigned int nbPrimtives() const;
	void fillDS(std::vector<DepthSort>& vds, unsigned int idObj) const;
};

//class SvgPolyline: public SvgObj
//{
//public:
//	void save(std::ofstream& out);
//};
//
//class SvgPolygon: public SvgObj
//{
//protected:
//	Geom::Vec3f m_colorFill;
//public:
//	void setColorFill(const Geom::Vec3f& c);
//	void save(std::ofstream& out);
//};




class SVGOut
{
protected:
	std::ofstream* m_out;

	const glm::mat4& m_model;
	const glm::mat4& m_proj;
	glm::i32vec4 m_viewport;

	Geom::Vec3f global_color;
	float global_width;

	std::vector<SvgObj*> m_objs;
	SvgObj* m_current;


	unsigned int m_bbX0;
	unsigned int m_bbY0;
	unsigned int m_bbX1;
	unsigned int m_bbY1;



protected:
	void computeBB(unsigned int& a, unsigned int& b, unsigned int& c, unsigned& d);

public:

	/**
	 * Object that allow the rendering/exporting in svg file
	 * @param filename file name ended by .svg
	 * @param model the modelview matrix
	 * @param proj the projection matrix
	 */
	SVGOut(const std::string& filename, const glm::mat4& model, const glm::mat4& proj);

	/**
	 * destructor
	 * flush and close the file
	 */
	~SVGOut();

	void setColor(const Geom::Vec3f& col);

	void setWidth(float w);

	void closeFile();

	void beginPoints();
	void endPoints();
	void addPoint(const Geom::Vec3f& P);
	void addPoint(const Geom::Vec3f& P, const Geom::Vec3f& C);


	void beginLines();
	void endLines();
	void addLine(const Geom::Vec3f& P, const Geom::Vec3f& P2);
	void addLine(const Geom::Vec3f& P, const Geom::Vec3f& P2, const Geom::Vec3f& C);


	void beginStrings(float scalefactor = 1.0f);
	void endStrings();
	void addString(const Geom::Vec3f& P, const std::string& str);
	void addString(const Geom::Vec3f& P, const Geom::Vec3f& Q, const std::string& str);
	void addString(const Geom::Vec3f& P, const std::string& str, const Geom::Vec3f& C);

	void sortSimpleDepth( std::vector<DepthSort>& vds);

};




} // namespace SVG
} // namespace Utils
} // namespace CGogN


#endif