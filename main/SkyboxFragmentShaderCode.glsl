#version 330

in vec3 vertexFrag;

uniform samplerCube mapping;

out vec4 colorFrag;

void main()
{
	colorFrag = texture(mapping, vertexFrag);
}

