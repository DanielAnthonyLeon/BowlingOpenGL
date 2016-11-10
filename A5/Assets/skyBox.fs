#version 330

in vec3 TexCoords;
out vec4 colour;

uniform samplerCube skybox;

void main() {
	colour = texture(skybox, TexCoords);
	//colour = 0.000001 * colour + vec4(1.0, 0.0, 0.0, 1.0);
}
