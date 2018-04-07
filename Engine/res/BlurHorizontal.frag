#version 330 core

uniform struct Uniforms {
    int sample_count;
    float step_size;
} u;

uniform sampler2D u_color_texture;

layout(location = 0) out vec3 out_color;

void main() {
    ivec2 pixel_coords = ivec2(gl_FragCoord.xy);
    ivec2 offset = ivec2(0, 0);
    ivec2 step = ivec2(u.step_size, 0);

    out_color = texelFetch(u_color_texture, pixel_coords, 0).rgb;
    for (int i = 0; i < u.sample_count; ++i) {
        offset += step;
        out_color += texelFetch(u_color_texture, pixel_coords + offset, 0).rgb;
        out_color += texelFetch(u_color_texture, pixel_coords - offset, 0).rgb;
    }

    out_color /= float(2 * u.sample_count + 1);
}