#version 330 core

layout(location = 0) in highp vec3 in_coordinates;

void main() {
	gl_Position = vec4(in_coordinates, 1);
}
