#version 460

layout(local_size_x = 8, local_size_y = 8, local_size_z = 1) in;

layout(location = 0) uniform vec4 u_PrefilterThreshold;

layout(rgba16f, binding = 0) uniform readonly image2D inImage;
layout(rgba16f, binding = 1) uniform writeonly image2D outImage;

#define EPSILON 1e-5

// Quadratic color thresholding
vec4 QuadraticThreshold(vec4 color, in float threshold, in vec3 curve)
{
	// params:
	//     curve = (threshold - knee, knee * 2, 0.25 / knee)

	// Pixel brightness
	float br = max(color.r, max(color.g, color.g));

	// Under-threshold part : Quadratic curve
	float rq = clamp(br - curve.x, 0.0, curve.y);
	rq = curve.z * rq * rq;

	// Combine and apply the brightness response curve.
    color *= max(rq, br - threshold) / max(br, EPSILON);

	return color;
}


void main()
{
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	vec4 color = imageLoad(inImage, texelCoord);

	color = QuadraticThreshold(color, u_PrefilterThreshold.x, u_PrefilterThreshold.yzw);

	imageStore(outImage, texelCoord, color);
}
