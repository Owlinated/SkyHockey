#version 330 core

const int frame_count = 100;
const int target_frame_time = 17;
const int max_frame_time = 33;

uniform int u_frame_times[frame_count];
uniform int u_frame_times_offset;
uniform float u_scale;

layout(location = 0) out vec4 out_color;

void main() {
    // Find frame & time corresponding to pixel
    ivec2 coords = ivec2(gl_FragCoord.xy / u_scale);

    // Stop if x coordinate is outside of frame
    if (coords.x >= frame_count) {
        discard;
    }

    // Find index in ring buffer and time of matching frame
    int index = (coords.x + u_frame_times_offset) % frame_count;
    int frame_time = u_frame_times[index];

    if (coords.y == target_frame_time) {
        // Green line for target frame time (1f / 60fps)
        out_color = vec4(0, 0.75, 0, 1);
    } else if (coords.y == max_frame_time) {
        // Red line for max frame time (1f / 30fps)
        out_color = vec4(0.75, 0, 0, 1);
    } else if (coords.y <= frame_time) {
        // Fill region under frame time in transparent blue
        out_color = vec4(0, 0, 0.75, 0.5);
    } else {
        // Discard pixel otherwise
        discard;
    }
}
