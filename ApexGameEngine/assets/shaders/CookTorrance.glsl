#include "Globals3D.glsl"

vec3 fresnel_Schlick(in float cosTheta, in vec3 F0)
{
	//vec3 fresnel = F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
	vec3 fresnel = F0 + (1.0 - F0) * clamp(exp2((-5.55473 * cosTheta - 6.98316) * cosTheta), 0.0, 1.0);
	
	return fresnel;
}

vec3 fresnel_SchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    // vec3 fresnel = F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
    vec3 fresnel = F0 + (max(vec3(1.0 - roughness), F0) - F0) * clamp(exp2((-5.55473 * cosTheta - 6.98316) * cosTheta), 0.0, 1.0);

	return fresnel;
} 

float ndf_Phong(in vec3 N, in vec3 M, in float roughness)
{
	float NdotM = abs(dot(N, M));
	float ndf = ((roughness + 2) / TwoPI) * pow(NdotM, roughness);

	return ndf;
}

float ndf_Beckmann(in vec3 N, in vec3 M, in float roughness)
{
	float NdotM = dot(N, M);
	float NdotM2 = NdotM * NdotM;
	float NdotM4 = NdotM2 * NdotM2;
	return NdotM4;

	float alpha2 = roughness * roughness;
	float exponent = -exp((1 - NdotM2) / (alpha2 * NdotM2));

	float ndf = exponent / (PI * alpha2 * NdotM4);

	return ndf;
}

float ndf_GGX(in vec3 N, in vec3 H, in float roughness) // aka Trowbridge-Reitz
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;

	float NdotH = clamped_dot(N, H);
	float NdotH2 = NdotH * NdotH;

	float denom_term = NdotH2 * (alpha2 - 1.0) + 1.0;
	float denom_term2 = denom_term * denom_term;

	float ndf = alpha2 / (PI * denom_term2);

	return ndf;
}

float geom_GGX_1(in float NdotV, in float k) // Schlick
{
	float result = NdotV / (NdotV * (1.0 - k) + k);
	
	return result;
}

float geom_GGX(in vec3 L, in vec3 V, in vec3 N, in float roughness) // Smith
{
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float NdotV = clamped_dot(N, V);
	float NdotL = clamped_dot(N, L);

	float geom = geom_GGX_1(NdotV, k) * geom_GGX_1(NdotL, k);

	return geom;
}

float diffuse_OrenNayar(in float angleVN, in float angleLN, in float roughness)
{
	float alpha = max(angleVN, angleLN);
	float beta = min(angleVN, angleLN);
	float gamma = cos(angleVN - angleLN);

	float roughness2 = roughness * roughness;
	float A = 1 - 0.5 * roughness2 / (roughness2 + 0.57); // 0.33 in original eqn but we are approximating inter-reflections term as well
	float B = 0.45 * roughness2 / (roughness2 + 0.09);
	float C = sin(alpha) * tan(beta);

	float diffuse = A + (B * max(gamma, 0.0) * C);

	return diffuse;
	return 1.0;
}
