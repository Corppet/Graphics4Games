#version 330 core

out vec4 FragColor;

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

float getFogFactor(Fog params, float fogCoordinate);

void main()
{
    FragColor = vec4(1.0); // set alle 4 vector values to 1.0
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