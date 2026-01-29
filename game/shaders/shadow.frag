#version 330 core

layout ( location = 0 ) out float FragDepth_0;

in vec4 WorldPos;

void main()
{
    FragDepth_0 = gl_FragCoord.z;
}