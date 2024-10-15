#version 410 core

in vec2 TexCoords;

uniform sampler2D leather_ball;

out vec4 FragColor;

void main()
{
    // Sample the cube map using the direction of the fragment
    vec4 tex_color = texture(leather_ball, TexCoords);

}
