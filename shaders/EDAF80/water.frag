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
uniform sampler2D water_normal_map;

uniform float time;

void main()
{
	vec3 view_dir = normalize(camera_position - fs_in.vertex);

	//vec3 n = normalize(vec3(-fs_in.dH_dx, 1, -fs_in.dH_dz));
	vec3 t = normalize(vec3(1.0, fs_in.dH_dx, 0.0));
	vec3 b = normalize(vec3(0.0, fs_in.dH_dz, 1.0));
	vec3 n = normalize(vec3(-fs_in.dH_dx, 1.0, -fs_in.dH_dz));

	mat3 TBN = mat3(t, b, n);

	vec2 texScale = vec2(8.0, 4.0);
	float normalTime = mod(time, 100.0);
	vec2 normalSpeed = vec2(-0.05, 0.0);

	vec2 normalCoord0 = fs_in.tex_coords.xy * texScale + normalTime * normalSpeed;

	vec2 normalCoord1 = fs_in.tex_coords.xy * texScale * 2.0 + normalTime * normalSpeed * 4.0;

	vec2 normalCoord2 = fs_in.tex_coords.xy * texScale * 4.0 + normalTime * normalSpeed * 8.0;
	
	vec3 n0 = normalize(texture(water_normal_map, normalCoord0).rgb * 2.0 - 1.0);
	vec3 n1 = normalize(texture(water_normal_map, normalCoord1).rgb * 2.0 - 1.0);
	vec3 n2 = normalize(texture(water_normal_map, normalCoord2).rgb * 2.0 - 1.0);

	vec3 normal_bump = normalize(n0 + n1+ n2);

	vec3 normal_bump_world = TBN * normal_bump;

	float facing = 1.0 - max(dot(view_dir, normal_bump_world), 0.0);

	vec4 colorwater = mix(colordeep, colorshallow, facing);

	vec3 reflection = reflect(-view_dir, normal_bump_world);

	
	vec4 cubemap_pixel_color = texture(nissan_beach, reflection);

	//TODO: use values above to render fragment color.
	frag_color = colorwater + cubemap_pixel_color;
	//frag_color = cubemap_pixel_color;
	//frag_color = vec4(fs_in.tex_coords, 0.0);
}
