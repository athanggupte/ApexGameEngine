#version 460

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(location = 0) uniform vec2 srcResolution;
layout(location = 1) uniform vec2 dstResolution;
layout(location = 2) uniform float inLod;
layout(location = 3) uniform float filterRadius;
layout(location = 4) uniform int isFinal;

layout(binding = 0) uniform sampler2D inImage;
layout(rgba16f, binding = 1) uniform restrict image2D outImage;

void main()
{
	vec2 inTexelSize = 1.0 / srcResolution;

    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec2 inTexelCoord = (vec2(texelCoord) + 0.5) / dstResolution;

	float dx = filterRadius;// * inTexelSize.x;
	float dy = filterRadius;// * inTexelSize.y;

	vec3 a = textureLod(inImage, vec2(inTexelCoord.x - dx, inTexelCoord.y + dy), inLod).rgb;
	vec3 b = textureLod(inImage, vec2(inTexelCoord.x,      inTexelCoord.y + dy), inLod).rgb;
	vec3 c = textureLod(inImage, vec2(inTexelCoord.x + dx, inTexelCoord.y + dy), inLod).rgb;

	vec3 d = textureLod(inImage, vec2(inTexelCoord.x - dx, inTexelCoord.y), inLod).rgb;
	vec3 e = textureLod(inImage, vec2(inTexelCoord.x,      inTexelCoord.y), inLod).rgb;
	vec3 f = textureLod(inImage, vec2(inTexelCoord.x + dx, inTexelCoord.y), inLod).rgb;

	vec3 g = textureLod(inImage, vec2(inTexelCoord.x - dx, inTexelCoord.y - dy), inLod).rgb;
	vec3 h = textureLod(inImage, vec2(inTexelCoord.x,      inTexelCoord.y - dy), inLod).rgb;
	vec3 i = textureLod(inImage, vec2(inTexelCoord.x + dx, inTexelCoord.y - dy), inLod).rgb;

	vec3 value = e * 4.0;
	value += (b + d + f + h) * 2.0;
	value += (a + c + g + i);
	value /= 16.0;

	//if (isFinal != 1) {
		vec4 oldValue = imageLoad(outImage, texelCoord);
		value = oldValue.rgb + value;
	//}

	imageStore(outImage, texelCoord, vec4(value, 1.0));
}
