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

layout(location = 1) uniform float u_Roughness;
//layout(location = 2) uniform float u_MaxMipLevels;
layout(location = 3) uniform float u_ImageWidth;

#include <internal_assets/shaders/Globals3D.glsl>
#include <internal_assets/shaders/CookTorrance.glsl>
#include "ImportanceSampling.glsl"


#define NUM_SAMPLES 32768u


vec3 PrefilterEnvironmentMap(in vec3 N, in float roughness)
{
	vec3 R = N;
	vec3 V = R;

	vec3 up = N.z < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));
	
	vec3 prefilteredColor = vec3(0.0);
	float totalWeight = 0.0;

	for (uint i = 0u; i < NUM_SAMPLES; i++) {
		vec2 Xi = Hammersley2D(i, NUM_SAMPLES);
		vec4 angles = ImportanceSample_GGX(Xi, N, roughness);
		vec3 tangentSampleVec = vec3(angles.y * angles.z, angles.x * angles.z, angles.w);

		vec3 H = tangentSampleVec.x * right + tangentSampleVec.y * up + tangentSampleVec.z * N;
		float NdotH = clamped_dot(N, H);
		float HdotV = NdotH; // since V = N

		vec3 L = normalize(2.0 * HdotV * H - V);

		float NdotL = clamped_dot(N, L);

		if (NdotL > 0.0) {
		#if 1
			float D = ndf_GGX(N, H, roughness);
			float pdf = max(D * NdotH / (4 * HdotV), 1e-4);
			
			float saTexel = 4.0 * PI / (6.0 * u_ImageWidth * u_ImageWidth);
			float saSample = 1.0 / (NUM_SAMPLES * pdf + 1e-5);
			
			float mipLevel = roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

			prefilteredColor += SafeHDR(textureLod(u_EnvironmentMap, L, mipLevel).rgb) * NdotL;
		#else
			prefilteredColor += SafeHDR(texture(u_EnvironmentMap, L).rgb) * NdotL;
		#endif
			totalWeight      += NdotL;
		}
	}

	prefilteredColor /= max(totalWeight, 1e-3);

	return prefilteredColor;
}

void main()
{
	vec3 normal = normalize(v_ViewDir);

	vec3 prefilteredColor = PrefilterEnvironmentMap(normal, u_Roughness);

	o_Color = vec4(prefilteredColor.rgb, 1.0);
}
