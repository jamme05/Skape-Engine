#version 330 core

out vec4 FragColor;

in vec2 ScreenPos;

void main()
{
    FragColor = vec4( ScreenPos, 0, 1 );
}