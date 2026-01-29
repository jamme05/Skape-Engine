#version 450 core

out vec4 FragColor;

in vec2 ScreenPos;

uniform sampler2D Positions;
uniform sampler2D Normals;
uniform sampler2D Albedo;

#define SK_CONSTANT_DIRECTIONAL_LIGHT_COUNT 2
#define SK_CONSTANT_POINT_LIGHT_COUNT 2
#define SK_CONSTANT_SPOT_LIGHT_COUNT 2

struct DirectionalLight
{
    vec4 color;
    vec3 direction;
    int  shadow_cast_index;
};

struct PointLight
{
    vec3  color;
    float radius;
    vec3  position;
    int   shadow_cast_index;
};

struct SpotLight
{
    vec3  color;
    float inner_angle;
    vec3  direction;
    float outer_angle;
    vec3  position;
    int   shadow_cast_index;
};

struct ShadowCaster
{
    uvec2 atlas_start;
    uvec2 atlas_end;
    mat4  light_view_proj;
};

layout( std140 ) uniform _LightSettings
{
    uint directional_light_count;
    uint point_light_count;
    uint spot_light_count;
    // Flags: 0x01 = Directional extended, 0x02 = Point extended, 0x04 = Spot extended.
    // Extended means to use a structured buffer instead of the faster uniform buffer
    uint uses_extended;

    DirectionalLight constant_directional_light[ SK_CONSTANT_DIRECTIONAL_LIGHT_COUNT ];
    PointLight constant_point_light[ SK_CONSTANT_POINT_LIGHT_COUNT ];
    SpotLight constant_spot_light[ SK_CONSTANT_SPOT_LIGHT_COUNT ];
    
    uvec2 atlas_size;
    uvec2 padding;

};

layout( std430 ) buffer _directionalLights
{
    DirectionalLight directional_lights[];
};
layout( std430 ) buffer _pointLights
{
    PointLight point_lights[];
};
layout( std430 ) buffer _spotLights
{
    SpotLight spot_lights[];
};
layout( std430 ) buffer _shadowCasters
{
    ShadowCaster shadow_casters[];
};

void main()
{
    vec4 color = texture( Albedo, ScreenPos ).rgba;
    color = color + vec4( directional_lights[ 0 ].color.xyz + point_lights[ 0 ].color + spot_lights[ 0 ].color, 0 );
    FragColor = vec4( color.xyz, 1 ) * color.a;
}