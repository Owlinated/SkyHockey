#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormal_modelspace;

out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;
out vec4 ShadowCoord;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightInvDirection_worldspace;
uniform mat4 DepthBiasMVP;

void main() {
    vec4 position4_modelspace = vec4(vertexPosition_modelspace, 1);
    vec4 normal4_modelspace = vec4(vertexNormal_modelspace, 0);
    vec4 light_inv4_worldspace = vec4(LightInvDirection_worldspace, 0);

	gl_Position =  MVP * position4_modelspace;
	UV = vertexUV;
	ShadowCoord = DepthBiasMVP * position4_modelspace;
	Position_worldspace = (M * position4_modelspace).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - (V * M * position4_modelspace).xyz;
	LightDirection_cameraspace = (V * light_inv4_worldspace).xyz;

	// FIXME does not handle model scaling (use inverse tranpose instead)
	Normal_cameraspace = (V * M * normal4_modelspace).xyz;
}

