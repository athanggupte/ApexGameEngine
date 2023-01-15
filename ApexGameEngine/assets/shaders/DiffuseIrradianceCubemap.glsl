#type vertex
#version 450

const vec3 pos[4] = vec3[4](
	vec3(-1.0, -1.0, -1.0),
	vec3( 1.0, -1.0, -1.0),
	vec3( 1.0,  1.0, -1.0),
	vec3(-1.0,  1.0, -1.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

#define X vec3(1.0, 0.0, 0.0)
#define Y vec3(0.0,-1.0, 0.0)
#define Z vec3(0.0, 0.0, 1.0)

mat3 view[6] = mat3[6](
	// view : ( Right, Up, Front )
	// Right
	mat3( -Z,  Y, -X ),
	// Left
	mat3(  Z,  Y,  X ),
	// Top
	mat3(  X, -Z, -Y ),
	// Bottom
	mat3(  X,  Z,  Y ),
	// Back
	mat3(  X,  Y, -Z ),
	// Front
	mat3( -X,  Y,  Z )
);

#undef X
#undef Y
#undef Z
#undef W

// Fixed perxpective project matrix
// FOV : 90deg
// aspect ratio : 1
// near : 0.1
// far  : 10
mat4 projection = mat4 (
	// ( 1 / (aspect * tan(90/2)), 0, 0, 0 )
	vec4( 1.0, 0.0, 0.0, 0.0 ),
	// ( 0, 1 / tan(90/2), 0, 0 )
	vec4( 0.0, 1.0, 0.0, 0.0 ),
	// ( 0, 0, (- near - far) / (near - far), 1 )
	vec4( 0.0, 0.0, -10.1 / -9.9, 1.0 ),
	// ( 0, 0, 2 * near * far / ( near - far ), 0 )
	vec4( 0.0, 0.0, 2 / -9.9, 0.0 )
);

layout(location = 0) out vec3 v_ViewDir;

layout(location = 0) uniform int u_FaceIndex;

void main()
{
	int vert_idx = indices[gl_VertexID];
	vec3 position = pos[vert_idx];
	
	v_ViewDir = transpose(view[u_FaceIndex]) * normalize(position);

	gl_Position = vec4(position.xy, 1.0, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_ViewDir;

layout(binding = TEX_SLOT_ENV_Skybox) uniform samplerCube u_EnvironmentMap;

#include <internal_assets/shaders/Globals3D.glsl>
#include "ImportanceSampling.glsl"


#define NUM_SAMPLES 50000u
#define USE_HAMMERSLEY_SAMPLING

vec3 SphericalSamplingConvolution(in vec3 normal, in vec3 up, in vec3 right, float delta)
{
	vec3 irradiance = vec3(0.0);
	float numSamples = 0.0;
	for (float phi = 0.0; phi < 2.0 * PI; phi += delta) {
		for (float theta = 0.0; theta < 0.5 * PI; theta += delta) {
			// Spherical to Cartesian coords (int tangent space)
			vec3 tangentSampleVec = vec3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			// Tanget space to World space
			vec3 sampleVec = tangentSampleVec.x * right + tangentSampleVec.y * up + tangentSampleVec.z * normal;

			vec3 Li = SafeHDR(texture(u_EnvironmentMap, sampleVec).rgb);

			irradiance += Li * cos(theta) * sin(theta);
			numSamples++;
		}
	}

	irradiance = PI * irradiance * (1.0 / numSamples);

	return irradiance;
}

vec3 HammerlseySamplingConvolution(in vec3 normal, in vec3 up, in vec3 right)
{
	vec3 irradiance = vec3(0.0);
	for (uint i = 0u; i < NUM_SAMPLES; i++) {
		vec2 Xi = Hammersley2D(i, NUM_SAMPLES);
		vec4 angles = HemisphereSample_cos(Xi.x, Xi.y);
		vec3 tangentSampleVec = vec3(angles.y * angles.z, angles.x * angles.z, angles.w);
		vec3 sampleVec = tangentSampleVec.x * right + tangentSampleVec.y * up + tangentSampleVec.z * normal;

		vec3 Li = SafeHDR(textureLod(u_EnvironmentMap, sampleVec, 0.0).rgb);

		irradiance += Li * angles.z * angles.w;
	}

	irradiance = PI * irradiance * (1.0 / float(NUM_SAMPLES));

	return irradiance;
}

vec3 CalculateIrradiance(in vec3 normal)
{
	vec3 irradiance = vec3(0.0);

	vec3 up = vec3(0.0, 1.0, 0.0);
	vec3 right = normalize(cross(up, normal));
	up = normalize(cross(normal, right));
	

#if defined(USE_HAMMERSLEY_SAMPLING)
	irradiance = HammerlseySamplingConvolution(normal, up, right);
#else
	// Convolve by sampling about Spherical angles
	const float delta = 0.025;
	irradiance = SphericalSamplingConvolution(normal, up, right, delta);
#endif

	return irradiance;
}

void main()
{
	vec3 normal = normalize(v_ViewDir);

	vec3 irradiance = CalculateIrradiance(normal);

	o_Color = vec4(irradiance.rgb, 1.0);
}
