#version 330 core

layout ( location = 0 ) out vec3 gPosition;
layout ( location = 1 ) out vec3 gNormal;
layout ( location = 2 ) out vec4 gAlbedoSpec;

in vec4 WorldPos;
in vec3 WorldNormal;
in vec2 TexCoord;

uniform float aatempSize;

uniform sampler2D cdefaultTexture;
uniform sampler2D bdefaultTexture1;
uniform sampler2D adefaultTexture2;

void main()
{
    gPosition   = WorldPos.xyz;
    gNormal     = WorldNormal;
    gAlbedoSpec = aatempSize * texture( cdefaultTexture, TexCoord ) * texture( bdefaultTexture1, TexCoord ) * texture( adefaultTexture2, TexCoord );
}