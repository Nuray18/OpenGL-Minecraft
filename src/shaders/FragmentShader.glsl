#version 330 core

out vec4 FragColor;

in vec2 texCoord;

uniform sampler2D texture1;
uniform vec3 color;

void main()
{
    vec4 texColor = texture(texture1, texCoord);

    if (color == vec3(-1.0)) 
    {
        FragColor = texColor;
    } else 
    {
        FragColor = vec4(color, 1.0);
    }
};