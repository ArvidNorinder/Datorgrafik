layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
uniform mat4 vertex_model_to_world;
uniform mat4 normal_model_to_world;
uniform mat4 vertex_world_to_clip;
/* TODO: Add time uniform */

out VS_OUT {
	vec3 vertex;
	vec3 normal;
} vs_out;

float wave(vec2 position, vec2 direction, float amplitude, float frequency, float phase, float sharpness, float time)
{
	return amplitude * pow(sin((position.x * direction.x + position.y * direction.y)
		* frequency + phase * time) * 0.5 + 0.5, sharpness);
}
void main()
{
	vec3 displaced_vertex = vertex;
	displaced_vertex.y += wave(vertex.xz, vec2(-1.0, 0.0), /* fill in... */ );
	vs_out.vertex = vec3(vertex_model_to_world * vec4(displaced_vertex, 1.0));
	vs_out.normal = vec3(normal_model_to_world * vec4(normal, 0.0));

	gl_Position = vertex_world_to_clip * vertex_model_to_world * vec4(vertex, 1.0);
}
