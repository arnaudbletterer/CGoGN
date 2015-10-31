// ShaderScalarFieldReal::vertexShaderText

ATTRIBUTE vec3 VertexPosition;
ATTRIBUTE float VertexScalar;
uniform mat4 ModelViewProjectionMatrix;
VARYING_VERT vec3 color;
INVARIANT_POS;

void main ()
{
	gl_Position = ModelViewProjectionMatrix * vec4 (VertexPosition, 1.0);
	color = vec3(VertexScalar);
}
