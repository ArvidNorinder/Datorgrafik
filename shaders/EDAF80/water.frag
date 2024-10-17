#version 410

in VS_OUT {
	vec3 vertex;
	//vec3 normal;
	vec3 tex_coords;
	float dH_dx;
	float dH_dz;
} fs_in;

out vec4 frag_color;

uniform vec3 camera_position;

uniform vec4 colordeep;
uniform vec4 colorshallow;

uniform samplerCube nissan_beach;

void main()
{
	vec3 view_dir = normalize(camera_position - fs_in.vertex);

	vec3 n = normalize(vec3(-fs_in.dH_dx, 1, -fs_in.dH_dz));

	float facing = 1 - max(dot(view_dir, n), 0);

	vec4 colorwater = mix(colordeep, colorshallow, facing);

	vec3 reflection = reflect(-view_dir, n);

	//TODO: Read in cubemap
	vec4 cubemap_pixel_color = texture(nissan_beach, reflection);


	//TODO: use values above to render fragment color.
	frag_color = colorwater + cubemap_pixel_color;
	//frag_color = cubemap_pixel_color;
}
