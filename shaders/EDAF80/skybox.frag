#version 410 core

in vec3 TexCoords;

out vec4 FragColor;

uniform samplerCube skybox;

void main()
{
    // Sample the cube map using the direction of the fragment
    vec4 tex_color = texture(skybox, TexCoords);
	FragColor = tex_color;
}
