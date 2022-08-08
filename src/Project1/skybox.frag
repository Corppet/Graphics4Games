#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

// fog
struct Fog
{
    vec3 color;

	// linear
	float near;
	float far;

	// exponential
	float density;

	int equation; // 0 = linear, 1 = exponential
    bool enabled;
};
uniform Fog fog;

uniform samplerCube skybox;

float getFogFactor(Fog params, float fogCoordinate);

void main()
{    
    if (fog.enabled)
		FragColor = vec4(fog.color, 1.0);
	else
		FragColor = texture(skybox, TexCoords);
}