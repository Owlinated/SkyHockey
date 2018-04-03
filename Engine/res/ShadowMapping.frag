#version 330 core

uniform struct Uniforms {
    mat4 model;
    mat4 view;
    mat4 model_view_projection;
    mat4 depth_bias_model_view_projection;
    vec3 light_inv_direction_worldspace;
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

// Light emission properties
vec3 LightColor = vec3(1,1,1);
float LightPower = 1.0f;

// Material properties
vec3 MaterialDiffuseColor = texture(u_color_texture, v.texture_coords).rgb;
vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

void main() {
    vec3 shadow_coord_normalized = v.shadow_coords.xyz / v.shadow_coords.w;
    vec4 moments = texture(u_shadow_map, shadow_coord_normalized.xy);

    // Standard shadow map comparison
    float lit_factor = shadow_coord_normalized.z <= moments.x ? 1 : 0;

    // Variance shadow mapping
    float light_vsm_epsilon = 0.00001f;
    float E_x2 = moments.y;
    float Ex_2 = moments.x * moments.x;
    float variance = min(max(E_x2 - Ex_2, 0) + light_vsm_epsilon, 1);
    float m_d = (moments.x - shadow_coord_normalized.z);
    float p_max = variance / (variance + m_d * m_d);

    // Adjust the light color based on the shadow attenuation
    float visibility = max(lit_factor, p_max);

    vec3 normal_normalized_cameraspace = normalize(v.normal_cameraspace);
    vec3 light_normalized_cameraspace = normalize(v.light_direction_cameraspace);
    vec3 eye_normalized_cameraspace = normalize(v.camera_direction_cameraspace);
    vec3 reflection_direction = reflect(-light_normalized_cameraspace, normal_normalized_cameraspace);

    float cos_normal_light = clamp(dot(normal_normalized_cameraspace, light_normalized_cameraspace), 0, 1);
    float cos_eye_reflection = clamp(dot(eye_normalized_cameraspace, reflection_direction), 0, 1);

	// Compute ambient, diffuse, and specular color components
	out_color = MaterialAmbientColor
	    + visibility * MaterialDiffuseColor * LightColor * LightPower * cos_normal_light
	    + visibility * MaterialSpecularColor * LightColor * LightPower * pow(cos_eye_reflection,5);
}