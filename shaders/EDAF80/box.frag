#version 410

uniform sampler2D diffuse_texture;  // The texture you’ll bind
uniform int has_diffuse_texture;    // Boolean flag to check if a texture is applied

in VS_OUT {
    vec2 texcoord;  // Incoming texture coordinates from the vertex shader
} fs_in;

out vec4 frag_color;

void main()
{
    // Check if a diffuse texture is provided
    if (has_diffuse_texture == 1) {
        // Sample the texture using the UV coordinates (fs_in.texcoord)
        frag_color = texture(diffuse_texture, fs_in.texcoord);
    } else {
        // Fallback color if no texture is applied
        frag_color = vec4(1.0, 1.0, 0.0, 1.0);  // Yellow for now
    }
}
