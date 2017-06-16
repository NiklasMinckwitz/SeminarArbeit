#GEOMETRY SHADER
#GEOMETRY SHADER END

#VERTEX SHADER
#version 330 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_normal;
layout(location = 2) in vec2 vertex_uv;

out vec2 fragment_uv;

void main()
{
	gl_Position = vec4(vertex_position, 1.0f);
	fragment_uv = vertex_uv;
}
#VERTEX SHADER END

#FRAGMENT SHADER
#version 330 core
out vec4 color;
in vec2 fragment_uv;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_metallic_roughness;

struct PointLight {
	vec3 position;
	vec3 color;

	float intensity;

	float constant;
	float linear;
	float quadratic;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;

	float intensity;
	float bias;

	bool cast_shadows;
	mat4 light_space;
	sampler2D shadow_map;
	float shadow_texel_size;
};

const int NR_POINTLIGHTS = 32;
const int NR_DIRECTIONALLIGHTS = 32;

uniform int current_pointlights;
uniform int current_directionallights;

uniform PointLight pointlights[NR_POINTLIGHTS];
uniform DirectionalLight directionallights[NR_DIRECTIONALLIGHTS];

uniform vec3 view_position;

const float PI = 3.14159265359;
vec2 poisson_disk[4] = vec2[](
	vec2(-0.94201624, -0.39906216),
	vec2(0.94558609, -0.76890725),
	vec2(-0.094184101, -0.92938870),
	vec2(0.34495938, 0.29387760)
	);

float distribution_GGX(vec3 N, vec3 H, float roughness);
float geometry_schlick_GGX(float NdotV, float roughtness);
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness);
vec3 fresnel_schlick(float cosTheta, vec3 F0);

float directional_light_shadow_calculation(int l, vec4 pos, float bias);

void main()
{
	vec3 fragment_position = texture(g_position, fragment_uv).rgb;
	vec3 fragment_normal = texture(g_normal, fragment_uv).rgb;
	vec3 fragment_albedo = texture(g_albedo, fragment_uv).rgb;
	float fragment_metallic = texture(g_metallic_roughness, fragment_uv).r;
	float fragment_roughness = texture(g_metallic_roughness, fragment_uv).g;

	vec3 view_dir = normalize(view_position - fragment_position);

	vec3 f0 = vec3(0.04);
	f0 = mix(f0, fragment_albedo, fragment_metallic);
	vec3 lightning = fragment_albedo * vec3(0.0);

	for (int i = 0; i < current_directionallights; ++i) {
		vec3 light_dir = normalize(-directionallights[i].direction);
		vec3 h = normalize(view_dir + light_dir);
		vec3 radiance = directionallights[i].color;

		float ndf = distribution_GGX(fragment_normal, h, fragment_roughness);
		float g = geometry_smith(fragment_normal, view_dir, light_dir, fragment_roughness);
		vec3 f = fresnel_schlick(max(dot(h, view_dir), 0.0), f0);

		vec3 reflection = f;
		vec3 refraction = vec3(1.0) - reflection;
		refraction *= 1.0 - fragment_metallic;

		vec3 nominator = ndf * g * f;
		float denominator = 4 * max(dot(fragment_normal, view_dir), 0.0) * max(dot(fragment_normal, light_dir), 0.0) + 0.001;
		vec3 specular = nominator / denominator;

		float shadow = 1.0;
		float bias = clamp(directionallights[i].bias * tan(acos(dot(fragment_normal, light_dir))), 0, 0.01);
		if (directionallights[i].cast_shadows) {
			shadow = directional_light_shadow_calculation(i, directionallights[i].light_space * vec4(fragment_position, 1.0), bias);
		}

		float n_dot_l = max(dot(fragment_normal, light_dir), 0.0);
		lightning += (refraction * fragment_albedo / PI + specular) * radiance * n_dot_l * directionallights[i].intensity * shadow;
	}
	color = vec4(lightning, 1.0);

	/*for(int i = 0; i < current_directionallights; ++i) {
		vec3 ambient = fragment_albedo * vec3(0.001, 0.001, 0.001);

		vec3 diffuse = fragment_albedo * max(dot(fragment_normal, light_dir), 0.0) * directionallights[i].color;

		vec3 halfway_dir = normalize(light_dir + view_dir);
		float spec = pow(max(dot(fragment_normal, halfway_dir), 0.0), 16.0);
		vec3 specular = directionallights[i].color * spec * fragment_specular;
		
		ambient *= directionallights[i].intensity;
		diffuse *= directionallights[i].intensity;
		specular *= directionallights[i].intensity;
		
		float shadow = 1.0;
		float bias = max(0.0015 * (1.0 - dot(normal, light_dir)), 0.005);
		if (directionallights[i].cast_shadows) {
			shadow = directional_light_shadow_calculation(i, directionallights[i].light_space * vec4(fragment_position, 1.0), bias);
		}
		
		lightning += shadow  * (ambient + diffuse + specular);
	}*/

	/*for (int i = 0; i < current_pointlights; ++i) {
		vec3 light_dir = normalize(pointlights[i].position - fragment_position);
		
		vec3 ambient = vec3(0.0, 0.0, 0.0); // fragment_albedo * vec3(0.0, 0.0, 0.0);
		
		vec3 diffuse = fragment_albedo * max(dot(fragment_normal, light_dir), 0.0) * pointlights[i].color;

		vec3 halfway_dir = normalize(light_dir + view_dir);
		float spec = pow(max(dot(fragment_normal, halfway_dir), 0.0), 16.0);
		vec3 specular = pointlights[i].color * spec * fragment_specular;
		
		float distance = length(pointlights[i].position - fragment_position);
		float attenuation = pointlights[i].intensity / (pointlights[i].constant + pointlights[i].linear * distance + pointlights[i].quadratic * (distance * distance));
		
		ambient *= attenuation;
		diffuse *= attenuation;
		specular *= attenuation;
		
		lightning += 1.0 * (ambient + diffuse + specular);
	}*/
}

float directional_light_shadow_calculation(int l, vec4 pos, float bias) {
	const float NUM_SAMPLES = 16.0f;
	const float SAMPLES_START = (NUM_SAMPLES - 1.0f) / 2.0f;
	const float NUM_SAMPLES_SQUARED = NUM_SAMPLES * NUM_SAMPLES;
	
	vec3 proj_coords = pos.xyz / pos.w;
	proj_coords = proj_coords * 0.5 + 0.5;

	float result = 0.0f;
	for (float y = -SAMPLES_START; y <= SAMPLES_START; y += 1.0f) {
		for (float x = -SAMPLES_START; x <= SAMPLES_START; x += 1.0f) {
			vec2 offset = vec2(x, y) * directionallights[l].shadow_texel_size;

			vec2 pixel_pos = (proj_coords.xy + offset) / vec2(directionallights[l].shadow_texel_size) + vec2(0.5);
			vec2 frac_part = fract(pixel_pos);
			vec2 start_texel = (pixel_pos - frac_part) * vec2(directionallights[l].shadow_texel_size);

			float blTexel = step(proj_coords.z - bias, texture(directionallights[l].shadow_map, start_texel).r);
			float brTexel = step(proj_coords.z - bias, texture(directionallights[l].shadow_map, start_texel + vec2(directionallights[l].shadow_texel_size, 0.0)).r);
			float tlTexel = step(proj_coords.z - bias, texture(directionallights[l].shadow_map, start_texel + vec2(0.0, directionallights[l].shadow_texel_size)).r);
			float trTexel = step(proj_coords.z - bias, texture(directionallights[l].shadow_map, start_texel + vec2(directionallights[l].shadow_texel_size)).r);

			float mixA = mix(blTexel, tlTexel, frac_part.y);
			float mixB = mix(brTexel, trTexel, frac_part.y);

			result += mix(mixA, mixB, frac_part.x);
		}
	}
	return result / NUM_SAMPLES_SQUARED;

	
	/*float shadow = 1.0;
	if (texture(directionallights[l].shadow_map, proj_coords.xy).r < proj_coords.z - bias) {
		shadow = 0.0;
	}*/

	/*for (int i = 0; i < 4; ++i) {
		
		if (texture(directionallights[l].shadow_map, proj_coords.xy + poisson_disk[i] / 500.0).r < proj_coords.z - bias) {
			
			shadow -= 0.25;
		}
	}*/
	

	/*vec2 texel_size = 1.0 / textureSize(directionallights[l].shadow_map, 0);
	for (int x = -1; x <= 1; ++x) {
		for (int y = -1; y <= 1; ++y) {
			float closestDepth = texture(directionallights[l].shadow_map, proj_coords.xy + vec2(x, y) * texel_size).r;
			float currentDepth = proj_coords.z;
			shadow += currentDepth - bias > closestDepth ? 0.0 : 1.0;
		}
	}
	shadow /= 9.0;*/

	//return shadow;
}

float distribution_GGX(vec3 N, vec3 H, float roughness) {
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}

float geometry_schlick_GGX(float NdotV, float roughness) {
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
float geometry_smith(vec3 N, vec3 V, vec3 L, float roughness) {
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = geometry_schlick_GGX(NdotV, roughness);
	float ggx1 = geometry_schlick_GGX(NdotL, roughness);

	return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cosTheta, vec3 F0) {
	return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}
#FRAGMENT SHADER END