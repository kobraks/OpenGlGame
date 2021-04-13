#version 450 core

layout (triangles) in;
layout (line_strip, max_vertices = 3) out;

in VS_OUT 
{
    vec3 FragPosition;
    vec2 TexCoords;
} gs_in[];

void GenerateLine(int index)
{
    gl_Position = gl_in[index].gl_Position;
    EmitVertex();
}

void main()
{
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}