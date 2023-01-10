#version 460

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(location = 0) uniform vec2 srcResolution;
layout(location = 1) uniform vec2 dstResolution;
layout(location = 2) uniform float inLod;
layout(location = 3) uniform vec4 u_PrefilterThreshold;

layout(binding = 0) uniform sampler2D inImage;
layout(rgba16f, binding = 1) uniform writeonly image2D outImage;

#define EPSILON 1e-5

// Quadratic color thresholding
vec4 QuadraticThreshold(vec4 color, in float threshold, in vec3 curve)
{
	// params:
	//     curve = (threshold - knee, knee * 2, 0.25 / knee)

	// Pixel brightness
	float br = max(color.r, max(color.g, color.b));

	// Under-threshold part : Quadratic curve
	float rq = clamp(br - curve.x, 0.0, curve.y);
	rq = curve.z * rq * rq;

	// Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);

	return color;
}

void main()
{
	vec2 inTexelSize = 0.5 / srcResolution;

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 inTexelCoord = vec2(texelCoord * 2 + 1) / srcResolution;

	float dx = inTexelSize.x;
	float dy = inTexelSize.y;

    vec3 a = textureLod(inImage, vec2(inTexelCoord.x - 2*dx, inTexelCoord.y + 2*dy), inLod).rgb;
	vec3 b = textureLod(inImage, vec2(inTexelCoord.x,        inTexelCoord.y + 2*dy), inLod).rgb;
	vec3 c = textureLod(inImage, vec2(inTexelCoord.x + 2*dx, inTexelCoord.y + 2*dy), inLod).rgb;

	vec3 d = textureLod(inImage, vec2(inTexelCoord.x - 2*dx, inTexelCoord.y), inLod).rgb;
	vec3 e = textureLod(inImage, vec2(inTexelCoord.x,        inTexelCoord.y), inLod).rgb;
	vec3 f = textureLod(inImage, vec2(inTexelCoord.x + 2*dx, inTexelCoord.y), inLod).rgb;

	vec3 g = textureLod(inImage, vec2(inTexelCoord.x - 2*dx, inTexelCoord.y - 2*dy), inLod).rgb;
	vec3 h = textureLod(inImage, vec2(inTexelCoord.x,        inTexelCoord.y - 2*dy), inLod).rgb;
	vec3 i = textureLod(inImage, vec2(inTexelCoord.x + 2*dx, inTexelCoord.y - 2*dy), inLod).rgb;

	vec3 j = textureLod(inImage, vec2(inTexelCoord.x - 1*dx, inTexelCoord.y + 1*dy), inLod).rgb;
	vec3 k = textureLod(inImage, vec2(inTexelCoord.x + 1*dx, inTexelCoord.y + 1*dy), inLod).rgb;
	vec3 l = textureLod(inImage, vec2(inTexelCoord.x - 1*dx, inTexelCoord.y - 1*dy), inLod).rgb;
	vec3 m = textureLod(inImage, vec2(inTexelCoord.x + 1*dx, inTexelCoord.y - 1*dy), inLod).rgb;

	vec3 value = e * 0.125;
	value += (a + c + g + i) * 0.03125;
	value += (b + d + f + h) * 0.0625;
	value += (j + k + l + m) * 0.125;

	vec4 color = vec4(value, 1.0);

	if (inLod == -1.0) {
		color = QuadraticThreshold(color, u_PrefilterThreshold.x, u_PrefilterThreshold.yzw);
	}

	imageStore(outImage, texelCoord, color);
}
