#version 330 core

layout ( location = 0 ) in vec2 POSITION;

out vec2 ScreenPos;

void main()
{
    ScreenPos = POSITION;

    gl_Position = vec4( POSITION, 0, 1 );
}