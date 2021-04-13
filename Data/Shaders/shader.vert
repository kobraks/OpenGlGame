#version 450 core
layout (location = 0) in vec3 in_Position;
layout (location = 1) in vec2 in_TexCoords;
layout (location = 2) in vec4 in_Color;
layout (location = 3) in vec3 in_Normal;

uniform mat4 u_ProjectionViewMatrix;
uniform mat4 u_Transform;
uniform mat4 u_InvertedTransform;

out VS_OUT{
	vec4 Color;
	vec3 Normal;
	vec2 TexCoords;
	vec3 FragPosition;
} vs_out;

//out int o_IsNormal;

void main()
{
	vec3 fragPos = vec3(u_Transform * vec4(in_Position, 1.f));

	//o_IsNormal = 0;

	vs_out.Color = in_Color;
	vs_out.Normal = mat3(transpose(u_InvertedTransform)) * in_Normal;
	vs_out.TexCoords = in_TexCoords;
	vs_out.FragPosition = fragPos;

	gl_Position = u_ProjectionViewMatrix * vec4(fragPos, 1.f);
}