#version 330 core

uniform struct Uniforms {
    mat4 view;
    mat4 view_projection;
    mat4 depth_view_projection_window;
    vec3 light_position_worldspace;
} u;

uniform sampler2D u_deferred_0;
uniform sampler2D u_deferred_1;
uniform sampler2D u_deferred_2;
uniform sampler2D u_deferred_3;
uniform sampler2D u_shadow_map;
// FIXME work around hardcoded texture max
uniform sampler2D u_color_texture[8];

layout(location = 0) out vec3 out_color;

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

    // Light emission properties
    vec3 LightColor = vec3(1,1,1);
    float LightPower = 1.0f;

    // Material properties
    vec3 MaterialDiffuseColor = texture(u_color_texture[object_id], texture_coords).rgb;
    vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
    vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

    // todo debug, remove
    out_color = texelFetch(u_shadow_map, ivec2(gl_FragCoord.xy), 0).rgb;
    out_color = abs(texture(u_color_texture[object_id], texture_coords).rgb);

    vec4 shadow_coords = u.depth_view_projection_window * position_worldspace;
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

    // Adjust the light color based on the shadow attenuation
    float visibility = max(lit_factor, p_max);

    // todo maybe normalize before throwing w away?
    // todo some of these multiplications might be replaced (e.g. compute cameraspace coordinates once, do calcs there)
    vec3 normal_cameraspace = (u.view * normal_worldspace).xyz;
    vec3 normal_normalized_cameraspace = normalize(normal_cameraspace);
    vec4 light_direction_worldspace = vec4(u.light_position_worldspace, 1) - position_worldspace;
    vec3 light_direction_cameraspace = (u.view * light_direction_worldspace).xyz;
    vec3 light_normalized_cameraspace = normalize(light_direction_cameraspace);
    vec3 camera_direction_cameraspace = vec3(0, 0, 0) - (u.view * position_worldspace).xyz;
    vec3 eye_normalized_cameraspace = normalize(camera_direction_cameraspace);
    vec3 reflection_direction = reflect(-light_normalized_cameraspace, normal_normalized_cameraspace);

    float cos_normal_light = clamp(dot(normal_normalized_cameraspace, light_normalized_cameraspace), 0, 1);
    float cos_eye_reflection = clamp(dot(eye_normalized_cameraspace, reflection_direction), 0, 1);

	// Compute ambient, diffuse, and specular color components
	out_color = MaterialAmbientColor
	    + visibility * MaterialDiffuseColor * LightColor * LightPower * cos_normal_light
	    + visibility * MaterialSpecularColor * LightColor * LightPower * pow(cos_eye_reflection,5);
}