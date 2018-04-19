#version 330 core

struct Light {
    vec3 position_worldspace;
    vec3 color;
};

struct Material {
    vec3 ambient_multiplier;
    vec3 diffuse_multiplier;
    vec3 specular_multiplier;
};

uniform struct Uniforms {
    mat4 model;
    mat4 view;
    mat4 model_view_projection;
    mat4 depth_model_view_projection_window;
    float depth_attenuation;
    Light light;
    Material material;
} u;

uniform sampler2D u_color_texture;
uniform sampler2D u_shadow_map;

in struct VertexData {
    vec2 texture_coords;
    vec3 normal_cameraspace;
    vec3 camera_direction_cameraspace;
    vec3 light_direction_cameraspace;
    vec4 shadow_coords;
} v;

layout(location = 0) out vec3 out_color;

// Compute visibility with variance shadow mapping
float shadow_visibility(vec4 shadow_coords) {
    vec3 shadow_coord_normalized = shadow_coords.xyz / shadow_coords.w;
    // Normalize to [0,1]
    float depth = shadow_coords.z / u.depth_attenuation;
    // Move to [-1,1]
    depth *= 2;
    depth -= 1;

    vec4 moments = texture(u_shadow_map, shadow_coord_normalized.xy);

    // Standard shadow map comparison
    float lit_factor = depth <= moments.x ? 1 : 0;

    // Variance shadow mapping
    float light_vsm_epsilon = 0.00001f;
    float E_x2 = moments.y;
    float Ex_2 = moments.x * moments.x;
    float variance = min(max(E_x2 - Ex_2, 0) + light_vsm_epsilon, 1);
    float m_d = (moments.x - depth);
    float p_max = variance / (variance + m_d * m_d);
    // Reduce ligh bleed
    p_max = clamp((p_max - 0.3) / 0.7, 0.0, 1.0);
    return max(lit_factor, p_max);
}

void main() {
    vec3 normal_normalized_cameraspace = normalize(v.normal_cameraspace);
    vec3 light_normalized_cameraspace = normalize(v.light_direction_cameraspace);
    vec3 eye_normalized_cameraspace = normalize(v.camera_direction_cameraspace);
    vec3 reflection_direction = reflect(-light_normalized_cameraspace, normal_normalized_cameraspace);

    float cos_normal_light = clamp(dot(normal_normalized_cameraspace, light_normalized_cameraspace), 0, 1);
    float cos_eye_reflection = clamp(dot(eye_normalized_cameraspace, reflection_direction), 0, 1);

	vec4 material_color = texture(u_color_texture, v.texture_coords);
	float visibility = shadow_visibility(v.shadow_coords);
	out_color = material_color.rgb * u.light.color * (u.material.ambient_multiplier
	    + visibility * u.material.diffuse_multiplier * cos_normal_light
	    + visibility * u.material.specular_multiplier * pow(cos_eye_reflection, 5));
}