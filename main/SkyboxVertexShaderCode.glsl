#version 330

in layout(location = 0) vec3 position;

uniform mat4 projection;
uniform mat4 view;

out vec3 vertexFrag;

void main()
{
	vec4 newPosition = vec4(position, 1);
	gl_Position =  projection * view * newPosition;
	vertexFrag = position;
}

