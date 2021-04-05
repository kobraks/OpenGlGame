#version 330 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec4 in_Color;
layout (location = 3) in vec3 in_Normal;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out vec4 Color;
out vec3 Normal;
out vec2 TexCoords;

void main()
{
    //gl_Position = u_Projection * u_View * u_Model * vec4(aPos, 1.0f);
	//gl_Position = u_Projection * u_View * u_Model * vec4(in_Position, 1.f);
	gl_Position = u_ProjectionViewMatrix * u_Transform * vec4(in_Position, 1.f);
	Color = in_Color;
	Normal = in_Normal;
	TexCoords = in_TexCoords;
}