#version 330 core

out vec4 FragColor;

in vec2 ScreenPos;

uniform sampler2D Positions;
uniform sampler2D Normals;
uniform sampler2D Albedo;

void main()
{
    FragColor = vec4( texture( Albedo, ScreenPos ).xyz, 1 ); // + vec4( ScreenPos, 0, 0 );
}