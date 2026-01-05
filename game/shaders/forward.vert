#version 330 core

layout ( location = 0 ) in vec3 POSITION;
layout ( location = 1 ) in vec3 NORMAL;
layout ( location = 2 ) in vec2 TEXCOORD;

out vec4 WorldPos;
out vec3 WorldNormal;
out vec2 TexCoord;

layout( std140 ) uniform _Camera
{
    mat4 view_proj;
};

layout( std140 ) uniform _Object
{
    mat4 world;
    mat4 inverse_world;
};

mat4 WorldViewProj()
{
    return world * view_proj;
}

void main()
{
    vec4 pos = WorldViewProj() * vec4( POSITION, 1.0f );
    WorldPos = world * vec4( POSITION, 1.0f );

    WorldNormal = mat3( transpose( inverse_world ) ) * NORMAL;

    TexCoord = TEXCOORD;

    gl_Position = pos;
}