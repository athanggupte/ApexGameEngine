#define APEX_SKIP_ATTRIBUTES
#include "Globals3D.glh"

float clamped_dot(in vec3 a, in vec3 b)
{
	return max(dot(a, b), 0.0);
}

vec3 fresnel_Schlick(in vec3 L, in vec3 N, in vec3 F0)
{
	float LdotN = dot(L, N);
	float OneMinusLdotN = 1.0 - LdotN;

	//vec3 fresnel = F0 + (1.0 - F0) * pow(OneMinusLdotN, 5.0);
	vec3 fresnel = F0 + (1.0 - F0) * exp2((-5.55473 * LdotN - 6.98316) * LdotN);

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

float ndf_GGX(in vec3 N, in vec3 M, in float roughness) // aka Trowbridge-Reitz
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;

	float NdotM = clamped_dot(N, M);
	float NdotM2 = NdotM * NdotM;

	float denom_term = NdotM2 * (alpha2 - 1.0) + 1.0;
	float denom_term2 = denom_term * denom_term;

	float ndf = alpha2 / (PI * denom_term2);

	return ndf;
}

float geom_GGX_1(in vec3 V, in vec3 N, in float k)
{
	float NdotV = clamped_dot(N, V);

	float result = NdotV / (NdotV * (1.0 - k) + k);
	
	return result;
}

float geom_GGX(in vec3 L, in vec3 V, in vec3 H, in vec3 N, in float roughness)
{
	float r = (roughness + 1) / 2.0;
	float alpha = r * r;
	float k = alpha / 2.0;

	float geom = geom_GGX_1(L, N, k) * geom_GGX_1(V, N, k);

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
