#type vertex
#version 450 core

#include <internal_assets/shaders/ShaderDefines.h>
#include "Globals3D.glh"

layout(location = 0) uniform Camera {
	vec3 u_CameraPosition;
	mat4 u_ViewProjection;
};
layout(location = 1) uniform mat4 u_Model;
layout(location = 2) uniform mat4 u_NormalMat;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out vec3 v_Normal;
layout(location = 3) out vec4 v_Tangent;

void main()
{
	v_TexCoord = a_TexCoord;

	vec3 T = vec3(u_Model * vec4(a_Tangent.xyz, 0.0));
	vec3 N = vec3(u_Model * vec4(a_Normal, 0.0));
	// vec3 B = mat3(u_Model) * cross(N, T) * a_Tangent.w;

	v_Tangent = vec4(T.xyz, a_Tangent.w);
	v_Normal = N;

	vec4 position_WorldSpace = u_Model * vec4(a_Position, 1.0);
	vec4 position_ViewSpace = u_ViewProjection * position_WorldSpace;
	
	v_Position = position_WorldSpace.xyz;
	gl_Position = position_ViewSpace;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec2 v_TexCoord;
layout(location = 2) in vec3 v_Normal;
layout(location = 3) in vec4 v_Tangent;

layout(std140, location = 0) uniform Camera {
	vec3 u_CameraPosition;
	mat4 u_ViewProjection;
};

layout(binding = 1) uniform samplerCube Skybox;
layout(binding = 2) uniform sampler2D Albedo;
layout(binding = 3) uniform sampler2D Normal;
layout(binding = 4) uniform sampler2D Metallic;
layout(binding = 5) uniform sampler2D Roughness;

layout(location = 3) uniform float u_Roughness;
layout(location = 4) uniform vec3 u_LightPos;

#define PI 3.1415926538
#define TwoPI 6.2831853076
#define HalfPI 1.5707963269
#define PI2 9.8696044024

#define epsilon 1e-5

//const float metallic = 0.01;
const vec3 emissive = vec3(0.0);//vec3(10.4, 8.5, 1.2);
const vec3 lightColor = vec3(1000.0);
const vec3 dirLightColor = vec3(10.0, 9.0, 6.0);
const vec3 dirLightDir = vec3(-0.8, -1.0, -1.0);

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

float ndf_Phong(in vec3 N, in vec3 M)
{
	float NdotM = abs(dot(N, M));
	float ndf = ((u_Roughness + 2) / TwoPI) * pow(NdotM, u_Roughness);

	return ndf;
}

float ndf_Beckmann(in vec3 N, in vec3 M)
{
	float NdotM = dot(N, M);
	float NdotM2 = NdotM * NdotM;
	float NdotM4 = NdotM2 * NdotM2;
	return NdotM4;

	float alpha2 = u_Roughness * u_Roughness;
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

void main()
{
	vec3 albedo = texture(Albedo, v_TexCoord).rgb;
	float metallic = texture(Metallic, v_TexCoord).r;
	float roughness = texture(Roughness, v_TexCoord).r;
	vec3 normal = texture(Normal, v_TexCoord).rgb;
	normal =  normal * 2.0 - 1.0;

	vec3 n = normalize(v_Normal);
	vec3 t = normalize(v_Tangent.xyz - n * dot(v_Tangent.xyz, n));
	vec3 b = cross(n, t) * v_Tangent.w;

	vec3 N = mat3(t, b ,n) * normal;
	vec3 V = normalize(u_CameraPosition - v_Position);
	vec3 L = normalize(u_LightPos - v_Position);
	//vec3 L = -normalize(dirLightDir);
	vec3 H = normalize(L + V);

	float NdotL = clamped_dot(N, L);
	float NdotV = clamped_dot(N, V);

	float angleLN = acos(NdotL);
	float angleVN = acos(NdotV);

	vec3 F0 = vec3(0.35); // Fresnel base value for Dielectrics
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0); // Light equation

	float lightDistance = length(u_LightPos - v_Position);
	float attenuation = 1.0 / (lightDistance * lightDistance);
	vec3 Li = lightColor * attenuation * PI;
	//Li = dirLightColor * PI;

	vec3 F = fresnel_Schlick(H, N, F0);
	float D = ndf_GGX(N, H, roughness);
	float G = geom_GGX(L, V, H, N, roughness);

	vec3 num_Spec = F * D * G;
	float den_Spec = 4.0 * NdotL * NdotV + epsilon;

	vec3 specular = num_Spec / den_Spec;

	vec3 kD = vec3(1.0);// - F; // kS = F
	kD *= 1.0 - metallic;
	vec3 diffuse = (kD * albedo / PI) * diffuse_OrenNayar(angleVN, angleLN, roughness);

	Lo += (diffuse + specular) * Li * NdotL;



	// Skybox reflection
	vec3 env = texture(Skybox, vec3(V)).rgb;

	o_Color = vec4(Lo + env, 1.0);
}