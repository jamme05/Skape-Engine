#version 330 core

out float FragColor;



in vec4 WorldPos;

void main()
{
    FragColor = gl_FragCoord.z;
}