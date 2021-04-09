#version 450 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec4 in_Color;
layout (location = 3) in vec3 in_Normal;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;
uniform mat4 u_InvertedTransform;

out vec4 o_Color;
out vec3 o_Normal;
out vec2 o_TexCoords;
out vec3 o_FragPosition;

void main()
{
	o_FragPosition = vec3(u_Transform * vec4(in_Position, 1.f));
	o_Normal = mat3(transpose(u_InvertedTransform)) * in_Normal;

	o_Color = in_Color;
	o_TexCoords = in_TexCoords;

	gl_Position = u_ProjectionViewMatrix * vec4(o_FragPosition, 1.f);
}