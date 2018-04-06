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
    mat4 view;
    mat4 view_projection;
    mat4 depth_view_projection_window;
    Light light;
    Material materials[8];
} u;

uniform sampler2D u_deferred_0;
uniform sampler2D u_deferred_1;
uniform sampler2D u_deferred_2;
uniform sampler2D u_deferred_3;
uniform sampler2D u_shadow_map;
uniform sampler2D u_color_texture[8];

layout(location = 0) out vec3 out_color;

// Compute visibility with variance shadow mapping
float shadow_visibility(vec4 shadow_coords) {
    vec3 shadow_coord_normalized = shadow_coords.xyz / shadow_coords.w;
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
    return max(lit_factor, p_max);
}

// Project worldspace location to shadowspace and compute visiblity
float shadow_visibility_world(vec4 position_worldspace){
    vec4 shadow_coords = u.depth_view_projection_window * position_worldspace;
    return shadow_visibility(shadow_coords);
}

// Compute color at the current location
vec3 material_light_color(Material material, vec4 material_color, Light light, vec4 position_worldspace, vec4 normal_worldspace) {
    // todo some of these multiplications might be replaced (e.g. compute cameraspace coordinates once, do calcs there)
    vec4 light_direction_worldspace = vec4(light.position_worldspace, 1) - position_worldspace;
    vec4 light_direction_cameraspace = u.view * light_direction_worldspace;
    vec3 light_normalized_cameraspace = normalize(light_direction_cameraspace).xyz;
    vec4 normal_cameraspace = u.view * normal_worldspace;
    vec3 normal_normalized_cameraspace = normalize(normal_cameraspace).xyz;
    float cos_normal_light = clamp(dot(normal_normalized_cameraspace, light_normalized_cameraspace), 0, 1);

    vec3 camera_direction_cameraspace = vec3(0, 0, 0) - (u.view * position_worldspace).xyz;
    vec3 eye_normalized_cameraspace = normalize(camera_direction_cameraspace);
    vec3 reflection_direction = reflect(-light_normalized_cameraspace, normal_normalized_cameraspace);
    float cos_eye_reflection = clamp(dot(eye_normalized_cameraspace, reflection_direction), 0, 1);

    float visibility = shadow_visibility_world(position_worldspace);
	return material_color.rgb * light.color * (material.ambient_multiplier
        + visibility * material.diffuse_multiplier * cos_normal_light
        + visibility * material.specular_multiplier * pow(cos_eye_reflection, 5));
}

void main() {
    // Read values from deferred textures
    vec4 in_0 = texelFetch(u_deferred_0, ivec2(gl_FragCoord.xy), 0);
    vec4 in_1 = texelFetch(u_deferred_1, ivec2(gl_FragCoord.xy), 0);
    vec4 in_2 = texelFetch(u_deferred_2, ivec2(gl_FragCoord.xy), 0);
    vec4 in_3 = texelFetch(u_deferred_3, ivec2(gl_FragCoord.xy), 0);

    // Read properties written by DeferredPrepare.frag
    vec4 position_worldspace = vec4(in_0.xyz, 1);
    vec4 normal_worldspace = vec4(in_1.xyz, 0);
    vec2 texture_coords = in_2.xy;
    int object_id = int(in_2.z);
    vec2 velocity_cameraspace = in_3.xy;

    vec4 texture_color = texture(u_color_texture[object_id], texture_coords);
    Material material = u.materials[object_id];
    out_color = material_light_color(material, texture_color, u.light, position_worldspace, normal_worldspace);
}