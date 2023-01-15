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

layout(location = 0) out vec2 v_UV;

void main()
{
	int vert_idx = indices[gl_VertexID];
	vec3 position = pos[vert_idx];
	
	v_UV = vec2(position.xy) / 2.0 + 0.5;

	gl_Position = vec4(position.xy, 1.0, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_UV;

#include <internal_assets/shaders/Globals3D.glsl>
#include <internal_assets/shaders/CookTorrance.glsl>
#include "ImportanceSampling.glsl"

#define NUM_SAMPLES 32768u

vec2 ConvoluteBRDF(float NdotV, float roughness)
{
	float cosTheta = NdotV;
	float sinTheta = sqrt(1.0 - NdotV * NdotV);
	
	vec3 V = vec3(sinTheta, 0.0, cosTheta);
	vec3 N = vec3(0.0, 0.0, 1.0);

	vec3 up = N.z < 0.999 ? vec3(0.0, 0.0, 1.0) : vec3(1.0, 0.0, 0.0);
	vec3 right = normalize(cross(up, N));
	up = normalize(cross(N, right));

	float A = 0.0; // Scale
	float B = 0.0; // Bias

	for (uint i = 0u; i < NUM_SAMPLES; i++) {
		vec2 Xi = Hammersley2D(i, NUM_SAMPLES);
		vec4 angles = ImportanceSample_GGX(Xi, N, roughness);
		vec3 tangentSampleVec = vec3(angles.y * angles.z, angles.x * angles.z, angles.w);

		vec3 H = tangentSampleVec.x * right + tangentSampleVec.y * up + tangentSampleVec.z * N;
		float NdotH = max(H.z, 0.0);
		float HdotV = clamped_dot(H, V);

		vec3 L = normalize(2.0 * HdotV * H - V);

		float NdotL = max(L.z, 0.0);

		if (NdotL > 0.0) {
			// BRDF approximation for Geometry function
			float k = roughness * roughness / 2.0;
			float G = geom_GGX_1(NdotL, k) * geom_GGX_1(NdotV, k);
			float G_Vis = (G * HdotV) / (NdotH * NdotV);
			float Fc = pow(1.0 - HdotV, 5.0);

			A += (1.0 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	A /= float(NUM_SAMPLES);
	B /= float(NUM_SAMPLES);

	return vec2(A, B);
}

void main()
{
	vec2 integratedBRDF = ConvoluteBRDF(v_UV.x, v_UV.y);
	o_Color = vec4(integratedBRDF, 0.0, 1.0);
}
