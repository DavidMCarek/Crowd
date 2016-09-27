#version 400

out vec4 frag_colour;
in vec3 color;
in vec3 norm_vec;

void main () {

	vec3 light1 = normalize(vec3 (1, 1, 1));
	vec3 light2 = normalize(vec3 (-1, 1, -1));
	vec3 norm_n = normalize(norm_vec);
	float eff_light = (max(dot(light1, norm_n), 0.2) * 0.5) + (max(dot(light2, norm_n), 0.2) * 0.5);
	frag_colour = vec4 (color * eff_light, 1.0);

}