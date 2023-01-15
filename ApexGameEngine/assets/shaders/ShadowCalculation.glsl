float ShadowCalculation(in vec4 lightSpaceFragPos, in float NdotL)
{
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	projCoords = projCoords * 0.5 + 0.5;

#ifdef SHADOWMAP_USE_ARRAY_TEXTURES
	float closestDepth = texture(_EnvShadowMap, vec3(projCoords.xy, 0)).r;
#else
	float closestDepth = texture(_EnvShadowMap, projCoords.xy).r;
#endif
	float currentDepth = projCoords.z;

	float bias = max(0.005 * (1.0 - NdotL), 0.0005);

	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	if(projCoords.z > 1.0)
        shadow = 0.0;

	return shadow;
}

#define PCF_SAMPLES_SQRT 4.0
#define PCF_SAMPLE_MIN (PCF_SAMPLES_SQRT - 1.0) / 2.0

float ShadowCalculationPCF(in vec4 lightSpaceFragPos, in float NdotL)
{
	vec3 projCoords = lightSpaceFragPos.xyz / lightSpaceFragPos.w;
	projCoords = projCoords * 0.5 + 0.5;

	float currentDepth = projCoords.z;

	//float bias = max(0.005 * (1.0 - NdotL), 0.0005);
	float bias = 0.0005;

	float shadow = 0.0;
	
	if(projCoords.z > 1.0)
        return shadow;

	vec2 texelSize = 1.0 / textureSize(_EnvShadowMap, 0).xy;
	for (float x = -PCF_SAMPLE_MIN; x <= PCF_SAMPLE_MIN; x++) {
		for (float y = -PCF_SAMPLE_MIN; y <= PCF_SAMPLE_MIN; y++) {
#ifdef SHADOWMAP_USE_ARRAY_TEXTURES
			float pcfDepth = texture(_EnvShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, 0.0)).r;
#else
			float pcfDepth = texture(_EnvShadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
#endif
			float shadowFactor = currentDepth - bias > pcfDepth ? 1.0 : 0.0;
			shadow += shadowFactor;
		}
	}

	shadow /= PCF_SAMPLES_SQRT * PCF_SAMPLES_SQRT;
	return shadow;
}
