#version 330

// Model-Space coordinates
in vec3 position;
in vec3 normal;
in vec2 uvCoords;

struct LightSource {
    vec3 position;
    vec3 rgbIntensity;
};
uniform LightSource light;

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Perspective;

// Remember, this is transpose(inverse(ModelView)).  Normals should be
// transformed using this matrix instead of the ModelView matrix.
uniform mat3 NormalMatrix;

// For shadow mapping
uniform mat4 lightSpaceMatrix;

out VsOutFsIn {
	vec3 position_ES; // Eye-space position
	vec3 normal_ES;   // Eye-space normal
	vec2 uvCoords_ES;
	LightSource light;
	vec4 FragPosLightSpace;
} vs_out;

out vec4 ShadowCoord;

void main() {
	vec4 pos4 = vec4(position, 1.0);

	//-- Convert position and normal to Eye-Space:
	vs_out.position_ES = (View * Model * pos4).xyz;
	vs_out.normal_ES = normalize(NormalMatrix * normal);
	
	vs_out.uvCoords_ES = uvCoords;

	vs_out.light = light;

	gl_Position = Perspective * View * Model * vec4(position, 1.0);
	
	vs_out.FragPosLightSpace = lightSpaceMatrix * Model * pos4;
}
