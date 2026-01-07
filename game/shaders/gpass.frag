#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec4 WorldPos;
in vec3 WorldNormal;
in vec2 TexCoord;

void main()
{
    gPosition   = WorldPos.xyz;
    gNormal     = WorldNormal;
    gAlbedoSpec = vec4( TexCoord, 0.0f, 1.0f );
}