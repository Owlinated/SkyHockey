#version 330 core

uniform struct Uniforms {
    mat4 model;
    mat4 model_view_projection;
    vec2 velocity_cameraspace;
    int object_id;
} u;

in struct VertexData {
   vec2 texture_coords;
   vec3 position_worldspace;
   vec3 normal_worldspace;
} v;

uniform sampler2D u_color_texture;

layout(location = 0) out vec4 out_0;
layout(location = 1) out vec4 out_1;
layout(location = 2) out vec4 out_2;
layout(location = 3) out vec4 out_3;

void main() {
    out_0 = vec4(v.position_worldspace, u.object_id + 1);
    out_1 = vec4(v.normal_worldspace, 0);
    out_2 = texture(u_color_texture, v.texture_coords);
    out_3 = vec4(u.velocity_cameraspace, 0, 0);
}