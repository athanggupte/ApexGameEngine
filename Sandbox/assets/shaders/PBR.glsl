#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat4 u_LightSpace;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec3 v_Normal;
out vec4 v_LightSpacePosition;

void main()
{
	v_Position = vec3(u_Model * vec4(a_Position, 1.0));;
	v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y * -1.0);
	v_Normal   = mat3(transpose(inverse(u_Model))) * a_Normal;
	v_LightSpacePosition = u_LightSpace * vec4(v_Position, 1.0);
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec3 v_Position;
in vec2 v_TexCoord;
in vec3 v_Normal;
in vec4 v_LightSpacePosition;

uniform vec3 u_CameraPosition;

uniform sampler2D u_TextureAlbedo;
uniform sampler2D u_TextureNormal;
uniform sampler2D u_TextureMetallic;
uniform sampler2D u_TextureRoughness;
//uniform sampler2D u_TextureAO;
uniform sampler2D u_TextureLightDepth;

uniform vec3 u_LightPositions[2];
uniform vec3 u_LightColors[2];

const float PI = 3.14159265359;

float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness * roughness;
	float a2 = a * a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH * NdotH;

	float num = a2;
	float den = NdotH2 * (a2 - 1.0) + 1.0;
	den = PI * den * den;

	return num / den;
}

float GeometrySchlickGGX(float NdotV, float k)
{
	float den = NdotV * (1.0 - k) + k;

	return 1.0 / den;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	
	float r = roughness + 1.0;
	float k = (r * r) / 8.0;

	float GGXV = GeometrySchlickGGX(NdotV, k);
	float GGXL = GeometrySchlickGGX(NdotL, k);

	return (GGXL * GGXV) / 4.0;
}

vec3 FresnelSchlick(float HdotV, vec3 F0)
{
	return F0 + (1-F0) * pow(1.0 - HdotV, 5);
}

vec3 GetNormalFromMap()
{
	vec3 tangentNormal = texture(u_TextureNormal, v_TexCoord).xyz * 2.0 - 1.0;

	vec3 Q1 = dFdx(v_Position);
	vec3 Q2 = dFdy(v_Position);

	vec2 st1 = dFdx(v_TexCoord);
	vec2 st2 = dFdy(v_TexCoord);

	vec3 N = normalize(v_Normal);

	vec3 T = normalize(Q1 * st2.t - Q2 * st1.t);
	vec3 B = -normalize(cross(N, T));
	mat3 TBN = mat3(T, B, N);

	return normalize(TBN * tangentNormal);
}

/*float ShadowCalculation(vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = v_LightSpacePosition.xyz / v_LightSpacePosition.w;
    
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_TextureLightDepth, projCoords.xy).r; 
    
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	if(currentDepth > 1.0)
		return 0.0;
    
	float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);  
	//float bias = 0.005;

	// check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}*/

void main()
{
	vec3 albedo     = pow(texture(u_TextureAlbedo, v_TexCoord).rgb, vec3(2.2));
	float metallic  = texture(u_TextureMetallic, v_TexCoord).r;
	float roughness = texture(u_TextureRoughness, v_TexCoord).r;
	//float ao        = texture(u_TextureAO, v_TexCoord).r;

	vec3 N = GetNormalFromMap();
	vec3 V = normalize(u_CameraPosition - v_Position);

	vec3 F0 = vec3(0.04);
	F0 = mix(F0, albedo, metallic);

	vec3 Lo = vec3(0.0);
	for(int i = 0; i < 1; i++) {
		vec3 L = normalize(u_LightPositions[i] - v_Position);
		vec3 H = normalize(V + L);
		float distance = length(u_LightPositions[i] - v_Position);
		float attenuation = 1.0; // (distance * distance);
		vec3 radiance = u_LightColors[i] * attenuation * max(dot(N, L), 0.0);

		float NDF = DistributionGGX(N, H, roughness);
		float G   = GeometrySmith(N, V, L, roughness);
		vec3 F    = FresnelSchlick(max(dot(H, V), 0.0), F0);

		vec3 specular = NDF * G * F;

		vec3 kS = F;
		vec3 kD = vec3(1.0) - kS;

		kD *= 1.0 - metallic;

		//Lo += radiance;
		//Lo += vec3(specular);
		Lo += (kD * albedo / PI + specular) * radiance;
	}
	//o_Color = vec4(Lo, 1.0);
	
	vec3 ambient = vec3(0.03) * albedo; // * ao;
	
	vec3 L = normalize(u_LightPositions[0] - v_Position);

	// float shadow = ShadowCalculation(N, L);
	float shadow = 0.0;

	vec3 color = ambient + (vec3(1.0 - shadow) * Lo);
	
	color = color / (color + vec3(1.0));
	
	color = pow(color, vec3(1.0/2.2));

	o_Color = vec4(color, 1.0);
	
}
