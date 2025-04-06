#version 330 core

out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float alfa;

void main()
{
    vec4 color1 = texture(texture1, TexCoord); // İlk texture’dan renk al
    vec4 color2 = texture(texture2, TexCoord);

    FragColor = mix(color1, color2, alfa); // İki texture’ı karıştır (blend)
};