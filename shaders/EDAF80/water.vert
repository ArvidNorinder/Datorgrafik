layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 tex_coords;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;

uniform float time;

out VS_OUT {
	vec3 vertex;
	//vec3 normal;
	vec3 tex_coords;
	float dH_dx;
	float dH_dz;
} vs_out;

vec3 wave(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time)
{
	float sin_term = sin((position.x * direction.x + position.y * direction.y) * frequency + phase * time);
	float cos_term = cos((position.x * direction.x + position.y * direction.y) * frequency + phase * time);

	float y = amplitude * pow(sin_term * 0.5 + 0.5, sharpness);

	float dy_dx = amplitude * sharpness * pow(sin_term * 0.5 + 0.5, sharpness - 1) 
				  * 0.5 * cos_term * direction.x * frequency;

	float dy_dz = amplitude * sharpness * pow(sin_term * 0.5 + 0.5, sharpness - 1) 
              * 0.5 * cos_term * direction.y * frequency;

	return vec3(y, dy_dx, dy_dz);
}
void main()
{
	vec3 displaced_vertex = vertex;
	vec3 wave1 = wave(vertex.xz, vec2(-1.0, 0.0), 1.0, 0.2, 0.5, 2.0, time);
	vec3 wave2 = wave(vertex.xz, vec2(-0.7, 0.7), 0.5, 0.4, 1.3, 2.0, time);

	//Storing the values needed for lighting in the fragment shader
	vs_out.dH_dx = wave1.y + wave2.y;
	vs_out.dH_dz = wave1.z + wave2.z;

	displaced_vertex.y += wave1.x + wave2.x;
	vs_out.vertex = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0));
	//vs_out.normal = vec3(normal_model_to_world * vec4(normal, 0.0));

	vs_out.tex_coords = tex_coords;

	/*TODO: Now use normal mapping.
	1. load in normal map as a sampler2D? or how was it loaded in again?
	2. Send the normals onward to fragment shader.
	3. Scale and normalize 
	2. create the TBN matrix and send it onward to fragment shader and normalize it there
	3. Use TBN matrix to transform normals from normal mapping
	*/
	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(displaced_vertex, 1.0);
}
