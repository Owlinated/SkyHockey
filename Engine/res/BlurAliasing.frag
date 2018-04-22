#version 430 core
// Implement FXAA: https://developer.download.nvidia.com/assets/gamedev/files/sdk/11/FXAA_WhitePaper.pdf

#define FXAA_EDGE_THRESHOLD 0.125
#define FXAA_EDGE_THRESHOLD_MIN 0.0625

#define FXAA_SUBPIX_TRIM 0.25
#define FXAA_SUBPIX_CAP 0.75
#define FXAA_SUBPIX_TRIM_SCALE (1.0/(1.0 - FXAA_SUBPIX_TRIM))

#define FXAA_SEARCH_STEPS 16

uniform bool u_anti_alias;
uniform bool u_show_edges;

uniform sampler2D u_color_texture;

out vec3 out_color;

float luma(vec3 color) {
    // Fast, as suggested by nvidia
    return color.g * (0.587/0.299) + color.r;

    // Slow but precise: https://www.w3.org/TR/AERT/#color-contrast
    // return dot(color, vec3(0.299, 0.587, 0.114));
}

vec3 fxaa() {
    ivec2 frag_coord = ivec2(gl_FragCoord.xy);

    // Find colors
    vec3 color_center = texelFetch(u_color_texture, frag_coord, 0).rgb;
    vec3 color_north = texelFetch(u_color_texture, frag_coord + ivec2(0, -1), 0).rgb;
    vec3 color_west = texelFetch(u_color_texture, frag_coord + ivec2(-1, 0), 0).rgb;
    vec3 color_east = texelFetch(u_color_texture, frag_coord + ivec2(1, 0), 0).rgb;
    vec3 color_south = texelFetch(u_color_texture, frag_coord + ivec2(0, 1), 0).rgb;

    vec3 color_north_west = texelFetch(u_color_texture, frag_coord + ivec2(-1, -1), 0).rgb;
    vec3 color_north_east = texelFetch(u_color_texture, frag_coord + ivec2(1, -1), 0).rgb;
    vec3 color_south_west = texelFetch(u_color_texture, frag_coord + ivec2(-1, 1), 0).rgb;
    vec3 color_south_east = texelFetch(u_color_texture, frag_coord + ivec2(1, 1), 0).rgb;

    // Find perceived brightness
    float luma_center = luma(color_center);
    float luma_north = luma(color_north);
    float luma_west = luma(color_west);
    float luma_east = luma(color_east);
    float luma_south = luma(color_south);
    float luma_north_west = luma(color_north_west);
    float luma_north_east = luma(color_north_east);
    float luma_south_west = luma(color_south_west);
    float luma_south_east = luma(color_south_east);

    float luma_min = min(luma_center,
                         min(min(luma_north, luma_west),
                             min(luma_east, luma_south)));
    float luma_max = max(luma_center,
                         max(max(luma_north, luma_west),
                             max(luma_east, luma_south)));
    float range = luma_max - luma_min;

    // Detect local contrast, ignore dark areas
    if (range < max(FXAA_EDGE_THRESHOLD_MIN, luma_max * FXAA_EDGE_THRESHOLD)) {
        return color_center;
    }

    if (u_show_edges) {
        return vec3(1, 0, 0);
    }

    // Determine lowpass value and amount
    float luma_lowpass = (luma_north + luma_west + luma_east + luma_south) * 0.25;
    float range_lowpass = abs(luma_lowpass - luma_center);
    float blend_lowpass = max(0.0, (range_lowpass / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
    blend_lowpass = min(FXAA_SUBPIX_CAP, blend_lowpass);

    vec3 color_lowpass = (color_center + color_north + color_west + color_east + color_south
              + color_north_west + color_north_east + color_south_west + color_south_east) * (1.0 / 9.0);

    // Skip rest of FXAA and just mix lowpass with pixel value
    return mix(color_lowpass, color_center, blend_lowpass);

    // TODO implement rest of FXAA
}

void main(void)
{
    if (u_anti_alias) {
        out_color = fxaa();
    } else {
        out_color = texelFetch(u_color_texture, ivec2(gl_FragCoord.xy), 0).rgb;
    }
}