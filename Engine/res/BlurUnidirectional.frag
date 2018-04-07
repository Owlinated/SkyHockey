#version 330 core

uniform struct Uniforms {
    int sample_count;
    int horizontal_step;
    int vertical_step;
    float falloff;
} u;

uniform sampler2D u_color_texture;

layout(location = 0) out vec4 out_color;

void main() {
    ivec2 pixel_coords = ivec2(gl_FragCoord.xy);
    ivec2 offset = ivec2(0, 0);
    ivec2 step_size = ivec2(u.horizontal_step, u.vertical_step);
    float intensity = 1.0f;
    float intensity_sum = 1.0f;

    out_color = texelFetch(u_color_texture, pixel_coords, 0);
    for (int i = 0; i < u.sample_count; ++i) {
        offset += step_size;
        intensity *= u.falloff;

        out_color += intensity * texelFetch(u_color_texture, pixel_coords + offset, 0);
        out_color += intensity * texelFetch(u_color_texture, pixel_coords - offset, 0);
        intensity_sum += 2 * intensity;
    }

    out_color /= intensity_sum;
}