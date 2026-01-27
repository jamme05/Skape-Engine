#version 330 core

out vec4 FragColor;

in vec2 ScreenPos;

uniform sampler2D Positions;
uniform sampler2D Normals;
uniform sampler2D Albedo;

layout( std140 ) uniform lightSettings
{
}

struct DirectionalLight
{
    vec4 color;
    vec3 direction;
    int  shadow_cast_index;
}

struct PointLight
{
    vec3  color;
    float radius;
    vec3  position;
    int   shadow_cast_index;
}

struct SpotLight
{
    vec3  color;
    float inner_angle;
    vec3  direction;
    float outer_angle;
    vec3  position;
    int   shadow_cast_index;
}

struct SpotLight
{
    uvec2 atlas_start;
    uvec2 atlas_end;
    mat4  light_view_proj;
}

layout( std430 ) buffer directionalLights
{
    DirectionalLight directional_lights[];
}
layout( std430 ) buffer pointLights
{
    DirectionalLight directional_lights[];
}
layout( std430 ) buffer spotLights
{
    DirectionalLight directional_lights[];
}

void main()
{
    FragColor = vec4( texture( Albedo, ScreenPos ).xyz, 1 );
}