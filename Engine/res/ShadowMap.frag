#version 330 core

uniform struct Uniforms {
    mat4 model_view_projection;
    float depth_attenuation;
} u;

layout(location = 0) out vec4 out_color;

void main() {
    float depth = gl_FragCoord.z / gl_FragCoord.w;
    // Normalize to [0,1]
    depth /= u.depth_attenuation;
    float squared = depth * depth;
    // Move to [-1,1]
    depth = (depth * 2) - 1;
    squared = (squared * 2) - 1;

    // TODO two components are unused, these might be used to store higher precision moments
	out_color = vec4(depth, squared, 0, 0);
}
