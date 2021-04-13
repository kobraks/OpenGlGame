#version 450 core

out vec4 FragColor;

uniform vec3 u_CameraPos;
uniform vec3 u_CameraFront;

in VS_OUT
{
    vec3 Normal;
    vec3 FragPosition;
} fs_in;

void main()
{
    vec3 viewDir = normalize(u_CameraPos - fs_in.FragPosition);


    FragColor = vec4(1.f, dot(viewDir, fs_in.Normal), 0.f, 1.f);
}