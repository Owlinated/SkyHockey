#version 330 core

uniform struct Uniforms {
    mat4 model;
    mat4 model_view_projection;
    vec2 velocity_cameraspace;
    int object_id;
} u;

layout(location = 0) in vec3 in_position_modelspace;
layout(location = 1) in vec2 in_texture_coords;
layout(location = 2) in vec3 in_normal_modelspace;

out struct VertexData {
    vec2 texture_coords;
    vec3 position_worldspace;
    vec3 normal_worldspace;
} v;

void main() {
    vec4 position_modelspace = vec4(in_position_modelspace, 1);

	gl_Position = u.model_view_projection * position_modelspace;
	v.position_worldspace = (u.model * position_modelspace).xyz;
	v.texture_coords = in_texture_coords;
	// FIXME does not handle model scaling (use inverse tranpose instead)
	v.normal_worldspace = (u.model * vec4(in_normal_modelspace, 0)).xyz;
}

