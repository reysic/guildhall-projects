#version 410 core

uniform mat4 gModel;
uniform mat4 gView;
uniform mat4 gProj;

in vec3 inPosition;
in vec4 inColor;
in vec2 inUV0;

out vec4 passColor;
out vec2 passUV0;
out vec3 passPosition;

void main (void)
{
	vec4 pos = vec4(inPosition, 1.f);
	passColor = inColor;
	passPosition = (gModel * pos).xyz;
	passUV0 = inUV0;

	
	//pos = pos * gModel * gView * gProj;//collumn major
	pos = gProj * gView * gModel * pos; //Row Major
	gl_Position = pos;
}