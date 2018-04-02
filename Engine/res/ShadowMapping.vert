#version 330 core

uniform struct Uniforms {
    mat4 model;
    mat4 view;
    mat4 model_view_projection;
    mat4 depth_bias_model_view_projection;
    vec3 light_inv_direction_worldspace;
} u;

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out struct VertexData {
    vec2 texture_coords;
    vec3 normal_cameraspace;
    vec3 camera_direction_cameraspace;
    vec3 light_direction_cameraspace;
    vec4 shadow_coords;
} v;

void main() {
    vec4 position4_modelspace = vec4(vertexPosition_modelspace, 1);
    vec4 normal4_modelspace = vec4(vertexNormal_modelspace, 0);
    vec4 light_inv4_worldspace = vec4(u.light_inv_direction_worldspace, 0);

	gl_Position =  u.model_view_projection * position4_modelspace;
	v.texture_coords = vertexUV;
	v.shadow_coords = u.depth_bias_model_view_projection * position4_modelspace;
	v.camera_direction_cameraspace = vec3(0, 0, 0) - (u.view * u.model * position4_modelspace).xyz;
	v.light_direction_cameraspace = (u.view * light_inv4_worldspace).xyz;

	// FIXME does not handle model scaling (use inverse tranpose instead)
	v.normal_cameraspace = (u.view * u.model * normal4_modelspace).xyz;
}

