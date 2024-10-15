#version 410 core
layout (location = 0) in vec3 aPos; // Vertex position at location 0

out vec3 TexCoords;

uniform mat4 vertex_world_to_clip;
uniform mat4 vertex_model_to_world;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Remove the translation part of the view matrix to keep the skybox centered around the camera
    mat4 viewWithoutTranslation = mat4(mat3(view));

    // Transform the vertex position (world to clip transformation)
    gl_Position = projection * viewWithoutTranslation * vertex_model_to_world * vec4(aPos, 1.0);
	//gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(aPos, 1.0);


    // Pass the direction to the fragment shader
    TexCoords = aPos;
}
