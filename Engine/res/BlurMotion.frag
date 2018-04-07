#version 330 core

uniform struct Uniforms {
    int sample_count;
    float step_size;
} u;

uniform sampler2D u_color_texture;
uniform sampler2D u_velocity_map;

layout(location = 0) out vec3 out_color;

void main() {
    ivec2 pixel_coords = ivec2(gl_FragCoord.xy);
    ivec2 velocity = ivec2(u.step_size * texelFetch(u_velocity_map, pixel_coords, 0).xy);

    // TODO Test if this block helps performance
    if(velocity == vec2(0,0)) {
        out_color = texelFetch(u_color_texture, pixel_coords, 0).rgb;
        return;
    }

    out_color = vec3(0, 0, 0);
    for (int i = 0; i < u.sample_count; ++i) {
        pixel_coords += velocity;
        out_color += texelFetch(u_color_texture, pixel_coords, 0).rgb;
    }

    out_color /= float(u.sample_count);
}