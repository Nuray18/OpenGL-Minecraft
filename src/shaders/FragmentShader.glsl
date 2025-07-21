#version 330 core

out vec4 FragColor;

in float DebugFlag;
in vec2 texCoord;

uniform sampler2D texture1;

void main()
{
    if (DebugFlag < 0.5)
    {
        FragColor = texture(texture1, texCoord);
    }
};