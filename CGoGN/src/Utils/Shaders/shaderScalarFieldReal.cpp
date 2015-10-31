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
#define CGoGN_UTILS_DLL_EXPORT 1
#include "Utils/Shaders/shaderScalarFieldReal.h"

namespace CGoGN
{

namespace Utils
{

#include "shaderScalarFieldReal.vert"
#include "shaderScalarFieldReal.frag"

ShaderScalarFieldReal::ShaderScalarFieldReal()
{
	std::string glxvert(GLSLShader::defines_gl());
	glxvert.append(vertexShaderText);

	std::string glxfrag(GLSLShader::defines_gl());
	glxfrag.append(fragmentShaderText);

	loadShadersFromMemory(glxvert.c_str(), glxfrag.c_str());
}

unsigned int ShaderScalarFieldReal::setAttributePosition(VBO* vbo)
{
	m_vboPos = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexPosition", vbo);
	unbind();
	return id;
}

unsigned int ShaderScalarFieldReal::setAttributeScalar(VBO* vbo)
{
	m_vboScal = vbo;
	bind();
	unsigned int id = bindVA_VBO("VertexScalar", vbo);
	unbind();
	return id;
}

void ShaderScalarFieldReal::restoreUniformsAttribs()
{
	bind();
	bindVA_VBO("VertexPosition", m_vboPos);
	bindVA_VBO("VertexScalar", m_vboScal);
	unbind();
}

} // namespace Utils

} // namespace CGoGN
