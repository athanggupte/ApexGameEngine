#type vertex
#version 450 core

layout(location = ATTRIB_LOC_Position)	in vec3 a_Position;
layout(location = ATTRIB_LOC_UV0)		in vec2 a_TexCoord;
layout(location = ATTRIB_LOC_Color)		in vec3 a_Color;
layout(location = ATTRIB_LOC_TexIndex)	in vec3 a_TexIndex;
layout(location = ATTRIB_LOC_Normal)	in vec3 a_Normal;
layout(location = ATTRIB_LOC_Tangent)	in vec4 a_Tangent;
layout(location = ATTRIB_LOC_Bitangent)	in vec3 a_Bitangent;
layout(location = ATTRIB_LOC_UV1)		in vec3 a_UV1;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec2 v_TexCoord;
layout(location = 2) out vec3 v_Normal;
layout(location = 3) out vec4 v_Tangent;
layout(location = 4) out vec4 v_LightSpaceFragPos;

#include "Globals3D.glh"

//layout(location = 0) uniform Camera {
//	vec3 u_CameraPosition;
//	mat4 u_ViewProjection;
//};


layout(location = 1) uniform mat4 u_Model;
layout(location = 2) uniform mat4 u_LightSpaceTransform;
layout(location = 3) uniform vec3 u_LightDir;
layout(location = 4) uniform vec3 u_LightPos;

#define SHADOW_NORMAL_OFFSET_BASE 1.0

vec4 ConvertToShadowSpace(in vec4 Pos_w, in vec3 N, in float NdotL, in mat4 lightVP)
{
	vec4 Pos_l;

	float texelSize = 1.0 / 2048.0;
	float normalOffset = clamp(1.0 - NdotL, 0.0, 1.0) * texelSize * SHADOW_NORMAL_OFFSET_BASE;
	vec4 shadowOffset = vec4(N * normalOffset, 0.0);

	vec4 shadowPos_w = Pos_w + shadowOffset;
	Pos_l = lightVP * shadowPos_w;

	return Pos_l;
}

void main()
{
	v_TexCoord = a_TexCoord;

	vec3 T = vec3(u_Model * vec4(a_Tangent.xyz, 0.0));
	vec3 N = vec3(u_Model * vec4(a_Normal, 0.0));
	// vec3 B = mat3(u_Model) * cross(N, T) * a_Tangent.w;

	v_Tangent = vec4(T.xyz, a_Tangent.w);
	v_Normal = N;

	vec4 position_WorldSpace = u_Model * vec4(a_Position, 1.0);
	vec4 position_ViewSpace = u_Camera.ViewProjectionMatrix * position_WorldSpace;
	

	vec3 L = -normalize(u_LightDir);
	float NdotL = dot(N, L);
	v_LightSpaceFragPos = ConvertToShadowSpace(position_WorldSpace, N, NdotL, u_LightSpaceTransform);
	
	
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
layout(location = 4) in vec4 v_LightSpaceFragPos;

#include "Globals3D.glh"

layout(binding = TEX_SLOT_ENV_Skybox) uniform samplerCube _EnvSkybox;
layout(binding = TEX_SLOT_MAT_Albedo) uniform sampler2D _MatAlbedo;
layout(binding = TEX_SLOT_MAT_Metallic) uniform sampler2D _MatMetallic;
layout(binding = TEX_SLOT_MAT_Roughness) uniform sampler2D _MatRoughness;
layout(binding = TEX_SLOT_MAT_Normal) uniform sampler2D _MatNormal;

#define SHADOWMAP_USE_ARRAY_TEXTURES
#ifdef SHADOWMAP_USE_ARRAY_TEXTURES
layout(binding = 5) uniform sampler2DArray _EnvShadowMap;
#else
layout(binding = 5) uniform sampler2D _EnvShadowMap;
#endif

//layout(location = 3) uniform vec3 u_LightDir;
//layout(location = 4) uniform vec3 u_LightPos;

layout(location = 5) uniform bool _bUseAlbedo;
layout(location = 6) uniform bool _bUseMetallic;
layout(location = 7) uniform bool _bUseRoughness;
layout(location = 8) uniform bool _bUseNormal;

layout(location = 9) uniform vec3 _altMatAlbedo;
layout(location = 10) uniform float _altMatMetallic;
layout(location = 11) uniform float _altMatRoughness;
const vec3 _altMatNormal = vec3(0.5, 0.5, 1.0);

//const float metallic = 0.01;
const vec3 emissive = vec3(0.0);//vec3(10.4, 8.5, 1.2);
//const vec3 lightColor = vec3(89.0, 12.0, 57.0);
//const vec3 dirLightColor = vec3(20.0, 17.0, 12.0);

#include "CookTorrance.glsl"
#include "ShadowCalculation.glsl"

vec3 CalculateBRDF(in vec3 N, in vec3 V, in vec3 L, in vec3 F0, in vec3 albedo, in float roughness, in float metallic, in vec3 Li)
{
	//vec3 L = -normalize(dirLightDir);
	vec3 H = normalize(L + V);

	float NdotL = clamped_dot(N, L);
	float NdotV = clamped_dot(N, V);

	float angleLN = acos(NdotL);
	float angleVN = acos(NdotV);

	// Calculate Specular component of BRDF	
	vec3 F = fresnel_Schlick(H, N, F0);
	float D = ndf_GGX(N, H, roughness);
	float G = geom_GGX(L, V, H, N, roughness);

	vec3 num_Spec = F * D * G;
	float den_Spec = 4.0 * NdotL * NdotV + epsilon;

	vec3 specular = num_Spec / den_Spec;

	// Calculate Diffuse component of BRDF
	vec3 kD = vec3(1.0);// - F; // kS = F
	kD *= 1.0 - metallic;
	vec3 diffuse = (kD * albedo / PI) * diffuse_OrenNayar(angleVN, angleLN, roughness);

	return (diffuse + specular) * Li * NdotL;
}

void main()
{
	// Set alternative parameter values
	vec3 albedo = _altMatAlbedo;
	float metallic = _altMatMetallic;
	float roughness = _altMatRoughness;
	vec3 normal = _altMatNormal;

	// Set parameters from textures if available
	if (_bUseAlbedo) albedo = texture(_MatAlbedo, v_TexCoord).rgb;
	if (_bUseMetallic) metallic = texture(_MatMetallic, v_TexCoord).r;
	if (_bUseRoughness) roughness = texture(_MatRoughness, v_TexCoord).r;
	if (_bUseNormal) normal = texture(_MatNormal, v_TexCoord).rgb;
	normal =  normal * 2.0 - 1.0;

	// Construct TBN matrix
	vec3 n = normalize(v_Normal);
	vec3 t = normalize(v_Tangent.xyz - n * dot(v_Tangent.xyz, n));
	vec3 b = cross(n, t) * v_Tangent.w;

	// Get world space vectors
	vec3 N = mat3(t, b ,n) * normal;
	vec3 V = normalize(u_Camera.Position.xyz - v_Position);

	// Set Fresnel value for material
	vec3 F0 = vec3(0.35); // Fresnel base value for Dielectrics
	F0 = mix(F0, albedo, metallic);

	// Light accumulator
	vec3 Lo = vec3(0.0);

	// for each Light :
	for (int i = 0; i < u_Lights.NumDirectionalLights; i++) 
	{
		// Light Type #1 (Directional)
		vec3 L = -normalize(u_Lights.DirectionalLights[i].Direction.xyz);
		vec3 Li = u_Lights.DirectionalLights[i].Color.rgb;

		// Calculate shadows
		// if (u_Lights.DirectionalLights[i].ShadowMapIndex > 0)
		//float shadow = ShadowCalculationPCF(v_LightSpaceFragPos, dot(N, L));
		float shadow = 0.0;

		Lo += (1.0 - shadow) * CalculateBRDF(N, V, L, F0, albedo, roughness, metallic, Li);
	}

	for (int i = 0; i < u_Lights.NumPointLights; i++) 
	{
		// Light Type #2 (Point)
		// Calculate Light vectors
		vec3 PtoL = u_Lights.PointLights[i].Position.xyz - v_Position;
		vec3 L = normalize(PtoL);
		float lightDistance = length(PtoL);

		// If the fragment is not in range of the light then do not calculate lighting from this light
		if (lightDistance < u_Lights.PointLights[i].Radius)
		{
			// Calculate base light color
			float attenuation = 1.0 / (lightDistance * lightDistance);
			vec3 Li = u_Lights.PointLights[i].Color.rgb * attenuation * PI;

			Lo += CalculateBRDF(N, V, L, F0, albedo, roughness, metallic, Li);
		}
	}


	// Final output fragment color
	o_Color = vec4(Lo, 1.0);
}