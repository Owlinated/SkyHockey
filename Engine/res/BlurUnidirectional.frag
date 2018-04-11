#version 330 core

uniform struct Uniforms {
    int sample_count;
    float[8] normalized_weights;
    int horizontal_step;
    int vertical_step;
} u;

uniform sampler2D u_color_texture;

layout(location = 0) out vec4 out_color;

void main() {
    ivec2 pixel_coords = ivec2(gl_FragCoord.xy);
    ivec2 offset = ivec2(0, 0);
    ivec2 step_size = ivec2(u.horizontal_step, u.vertical_step);

    out_color = u.normalized_weights[0] * texelFetch(u_color_texture, pixel_coords, 0);
    for (int i = 1; i < u.sample_count; ++i) {
        offset += step_size;
        float weight = u.normalized_weights[i];
        out_color += weight * texelFetch(u_color_texture, pixel_coords + offset, 0);
        out_color += weight * texelFetch(u_color_texture, pixel_coords - offset, 0);
    }
}