#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

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

in vec4 EyeSpacePos;
uniform Fog fog;

uniform sampler2D texture_diffuse1;

float getFogFactor(Fog params, float fogCoordinate);

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);

	// Apply fog calculation only if fog is enabled
    if(fog.enabled)
    {
      float fogCoordinate = abs(EyeSpacePos.z / EyeSpacePos.w);
      FragColor = mix(FragColor, vec4(fog.color, 1.0), getFogFactor(fog, fogCoordinate));
    }
}

float getFogFactor(Fog params, float fogCoordinate)
{
	float result = 0.0;
	if(params.equation == 0)
	{
		float fogLength = params.near - params.far;
		result = (params.far - fogCoordinate) / fogLength;
	}
	else if(params.equation == 1) 
    {
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}