#version 450 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

uniform mat4 u_Projection;
uniform float u_Magnitude;

in VS_OUT
{
    vec3 Normal;
    vec3 FragPosition;
} gs_in[];

out VS_OUT
{
    vec3 Normal;
    vec3 FragPosition;
} gs_out;

void GenerateLine(int index)
{
    gl_Position = u_Projection * gl_in[index].gl_Position;
    gs_out.Normal = gs_in[index].Normal;
    gs_out.FragPosition = gl_Position.xyz;
    EmitVertex();

    gl_Position = u_Projection * (gl_in[index].gl_Position + vec4(gs_in[index].Normal, 0.f) * u_Magnitude);
    gs_out.Normal = gs_in[index].Normal;
    gs_out.FragPosition = gl_Position.xyz;

    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}
