#version 410

layout (location = 0) in vec3 vertex;  // Vertex position
layout (location = 2) in vec2 texcoord;  // Texture coordinates

uniform mat4 vertex_model_to_world;
uniform mat4 vertex_world_to_clip;

out VS_OUT {
    vec2 texcoord;  // Output the texture coordinates to the fragment shader
} vs_out;

void main() {
    vs_out.texcoord = texcoord;  // Pass the texture coordinates through

    // Calculate final vertex position in clip space
    gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}
