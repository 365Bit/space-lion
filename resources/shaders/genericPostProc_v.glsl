#version 330

in vec3 v_position;
in vec2 v_uvCoord;

out vec2 uvCoord;

void main()
{
	uvCoord = v_uvCoord;
	gl_Position =  vec4(v_position, 1.0);
}