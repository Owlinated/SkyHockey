#version 330 core

uniform struct Uniforms {
    mat4 model_view_projection;
    float depth_attenuation;
} u;

layout(location = 0) in vec3 in_position_modelspace;

void main() {
	gl_Position = u.model_view_projection * vec4(in_position_modelspace, 1);
}
