// ShaderScalarFieldReal::fragmentShaderText

PRECISION;
uniform float minValue;
uniform float maxValue;
VARYING_FRAG vec3 color;
FRAG_OUT_DEF;

void main()
{
	gl_FragColor = vec4(color, 1.0);
}
