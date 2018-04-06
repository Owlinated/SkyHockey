#version 330 core

uniform struct Uniforms {
    mat4 model;
    mat4 view;
    mat4 model_view_projection;
    mat4 depth_window_model_view_projection;
    vec3 light_position_worldspace;
} u;

layout(location = 0) in vec3 in_position_modelspace;
layout(location = 1) in vec2 in_texture_coords;
layout(location = 2) in vec3 in_normal_modelspace;

out struct VertexData {
    vec2 texture_coords;
    vec3 normal_cameraspace;
    vec3 camera_direction_cameraspace;
    vec3 light_direction_cameraspace;
    vec4 shadow_coords;
} v;

void main() {
    vec4 position_modelspace = vec4(in_position_modelspace, 1);
    vec4 position_worldspace = u.model * position_modelspace;
    vec4 normal_modelspace = vec4(in_normal_modelspace, 0);
    vec4 light_position_worldspace = vec4(u.light_position_worldspace, 1);

	gl_Position =  u.model_view_projection * position_modelspace;
	v.texture_coords = in_texture_coords;
	v.shadow_coords = u.depth_window_model_view_projection * position_modelspace;
	v.camera_direction_cameraspace = vec3(0, 0, 0) - (u.view * position_worldspace).xyz;
	v.light_direction_cameraspace = (u.view * (light_position_worldspace - position_worldspace)).xyz;

	// FIXME does not handle model scaling (use inverse tranpose instead)
	v.normal_cameraspace = (u.view * u.model * normal_modelspace).xyz;
}

