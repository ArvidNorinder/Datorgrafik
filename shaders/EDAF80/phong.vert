#version 410 core
layout (location = 0) in vec3 aPos; // Vertex position at location 0
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texCoords;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 binormal;

uniform mat4 vertex_world_to_clip;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;

out vec2 TexCoords;

void main()
{
    // Transform the vertex position (world to clip transformation)
    gl_Position = aPos;


    TexCoords = texCoords.xy;
}
