#version 410 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec3 aNormal;   // Vertex normal

out vec3 FragPos;      // Fragment position in world space
out vec3 Normal;       // Normal vector in world space

uniform mat4 model;          // Model matrix (object to world transformation)
uniform mat4 view;           // View matrix (world to view transformation)
uniform mat4 projection;     // Projection matrix (view to clip transformation)

void main()
{
    // Transform the vertex position into world space and pass it to the fragment shader
    FragPos = vec3(model * vec4(aPos, 1.0));

    // Transform the normal vector to world space (by multiplying it with the inverse transpose of the model matrix)
    Normal = mat3(transpose(inverse(model))) * aNormal;

    // Output the final position of the vertex in clip space
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
