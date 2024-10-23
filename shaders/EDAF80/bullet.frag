#version 410

uniform sampler2D diffuse_texture;  // The 2D texture for the bullet
uniform int has_diffuse_texture;    // Flag to check if texture is applied

in VS_OUT {
    vec2 texcoord;  // Incoming texture coordinates
} fs_in;

out vec4 frag_color;

void main()
{
    if (has_diffuse_texture == 1) {
        frag_color = texture(diffuse_texture, fs_in.texcoord);  // Sample the texture
    } else {
        frag_color = vec4(0.0, 0.0, 0.0, 1.0);  // Default color (black) if no texture
    }
}
