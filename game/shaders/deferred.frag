#version 330 core

out vec4 FragColor;

in vec2 ScreenPos;

uniform sampler2D screenTexture;

void main()
{
    FragColor = vec4( texture( screenTexture, ScreenPos ).xyz, 1 ); // + vec4( ScreenPos, 0, 0 );
}