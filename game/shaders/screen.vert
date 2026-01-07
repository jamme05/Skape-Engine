#version 330 core

layout ( location = 0 ) in vec2 POSITION;

out vec2 ScreenPos;

void main()
{
    ScreenPos = POSITION;

    gl_Position = vec4( ( POSITION - vec2( 0.5, 0.5 ) ) * 2, 0, 1 );
}