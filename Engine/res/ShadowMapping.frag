#version 330 core

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;
in vec4 ShadowCoord;

layout(location = 0) out vec3 color;

uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform sampler2DShadow shadowMap;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

// Light emission properties
vec3 LightColor = vec3(1,1,1);
float LightPower = 1.0f;

// Material properties
vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

void main() {
	// Compute shadow
	float visibility = 1.0;
	int iterations = 8;
	for (int i=0; i<iterations; i++){
		vec3 shadow_coord_normalized = ShadowCoord.xyz / ShadowCoord.w;
		vec3 shadow_coord_offset = vec3(shadow_coord_normalized.xy + poissonDisk[i]/1000.0, shadow_coord_normalized.z);
        float shadowDepth = texture(shadowMap, shadow_coord_offset);
        visibility -= shadowDepth > shadow_coord_normalized.z ? 0.0 : 1.0 / iterations;
	}

    vec3 normal_normalized_cameraspace = normalize( Normal_cameraspace );
    vec3 light_normalized_cameraspace = normalize( LightDirection_cameraspace );
    vec3 eye_normalized_cameraspace = normalize(EyeDirection_cameraspace);
    vec3 reflection_direction = reflect(-light_normalized_cameraspace, normal_normalized_cameraspace);

    float cos_normal_light = clamp(dot(normal_normalized_cameraspace, light_normalized_cameraspace), 0, 1);
    float cos_eye_reflection = clamp(dot(eye_normalized_cameraspace, reflection_direction), 0, 1);

	// Compute ambient, diffuse, and specular color components
	color = MaterialAmbientColor
	    + visibility * MaterialDiffuseColor * LightColor * LightPower * cos_normal_light
	    + visibility * MaterialSpecularColor * LightColor * LightPower * pow(cos_eye_reflection,5);

}