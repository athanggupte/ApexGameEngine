#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normal;
layout(location = 3) in vec3 a_Tangent;
layout(location = 4) in vec3 a_Bitangent;

out VS_OUT
{
	vec3 Position;
	vec2 TexCoord;
	mat3 TBN;
} vs_out;

#define MAX_DIR_LIGHTS 1
#define MAX_POINT_LIGHTS 2

layout(std140, binding = 1) uniform RenderUniforms
{
	// -- Camera --
	mat4 Camera_Projection;
	mat4 Camera_View;
	vec4 Camera_Position;
	// -- Lights --
	vec4 DirLights_Direction[MAX_DIR_LIGHTS];
	vec4 DirLights_Color[MAX_DIR_LIGHTS];
	vec4 DirLights_Intensity[MAX_DIR_LIGHTS];
	
	vec4 PointLights_Position[MAX_POINT_LIGHTS];
	vec4 PointLights_Color[MAX_POINT_LIGHTS];
	vec4 PointLights_Intensity[MAX_POINT_LIGHTS];
	vec4 PointLights_Attenuation[MAX_POINT_LIGHTS];
	// -- Mesh --
	mat4 Mesh_Transform;
	mat3 Mesh_NormalMat;
} ub;

struct Mesh_Matrices
{
	mat4 Transform;
	mat4 NormalMat;
};

layout(std430, binding = 2) buffer RenderStorage
{
	Mesh_Matrices Mesh_Instance[];
} sb;

// layout(std430, binding = 1) buffer RenderStorage
// {
// 	mat4 Mesh_Transforms[];
// 	mat3 Mesh_NormalMat[];
// }ssb;

void main()
{
	vs_out.Position = vec3(sb.Mesh_Instance[gl_InstanceID].Transform * vec4(a_Position, 1.0));
	vs_out.TexCoord = vec2(a_TexCoord.x, 1.0 * a_TexCoord.y);
	
	vec3 T = normalize(mat3(sb.Mesh_Instance[gl_InstanceID].NormalMat) * a_Tangent);
	vec3 B = normalize(mat3(sb.Mesh_Instance[gl_InstanceID].NormalMat) * a_Bitangent);
	vec3 N = normalize(mat3(sb.Mesh_Instance[gl_InstanceID].NormalMat) * a_Normal);
	
	/*
	vec3 T = normalize(ub.Mesh_NormalMat * a_Tangent);
	vec3 B = normalize(ub.Mesh_NormalMat * a_Bitangent);
	vec3 N = normalize(ub.Mesh_NormalMat * a_Normal);
	*/
	/* Gramm-Shmidt re-orthogonalization
	vec3 T = normalize(ub.Mesh_NormalMat * a_Tangent);
	vec3 N = normalize(ub.Mesh_NormalMat * a_Normal);
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	*/
	vs_out.TBN = mat3(T, B, N);
	gl_Position = ub.Camera_Projection * ub.Camera_View * vec4(vs_out.Position, 1.0);
}

#type fragment
#version 450

in VS_OUT
{
	vec3 Position;
	vec2 TexCoord;
	mat3 TBN;
} fs_in;

#define MAX_DIR_LIGHTS 1
#define MAX_POINT_LIGHTS 2

// struct DirLight
// {
// 	vec4 Direction;
// 	vec3 Color;
// 	float DiffuseIntensity;
// 	float AmbientIntensity;
// 	float specularIntensity
// };

layout(std140, binding = 1) uniform RenderUniforms
{
	// -- Camera --
	mat4 Camera_Projection;
	mat4 Camera_View;
	vec4 Camera_Position;
	// -- Lights --
	vec4 DirLights_Direction[MAX_DIR_LIGHTS];
	vec4 DirLights_Color[MAX_DIR_LIGHTS];
	vec4 DirLights_Intensity[MAX_DIR_LIGHTS];
	
	vec4 PointLights_Position[MAX_POINT_LIGHTS];
	vec4 PointLights_Color[MAX_POINT_LIGHTS];
	vec4 PointLights_Intensity[MAX_POINT_LIGHTS];
	vec4 PointLights_Attenuation[MAX_POINT_LIGHTS];
	// -- Mesh --
	mat4 Mesh_Transform;
	mat3 Mesh_NormalMat;
} ub;

layout(binding = 0) uniform sampler2D Material_Albedo;
layout(binding = 1) uniform sampler2D Material_Roughness;
layout(binding = 2) uniform sampler2D Material_Normal;

out vec4 o_Color;

float shininess = 32.0;

#define DEBUG_COLOR vec4(0.92, 0.43, 0.87, 1.0)

vec3 CalculateDirectionalLightComponent(in vec3 normal, in vec3 viewDir, in int lIndex)
{
	vec3 lightDir = normalize(-ub.DirLights_Direction[lIndex].xyz); // lightDir : fragment -> light
	vec3 reflectDir = reflect(-lightDir, normal);
	
	// Ambient
	vec3 ambient = ub.DirLights_Color[lIndex].rgb * ub.DirLights_Intensity[lIndex].x;
	
	// Diffuse
	float diffuseFactor = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * ub.DirLights_Color[lIndex].rgb * ub.DirLights_Intensity[lIndex].y;
	
	// Specular
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularFactor * ub.DirLights_Color[lIndex].rgb * ub.DirLights_Intensity[lIndex].z;
	
	// Mixing
	vec3 finalColor = (ambient + diffuse + specular);
	
	return finalColor;
}

vec3 CalculatePointLightComponent(in vec3 normal, in vec3 viewDir, in int lIndex)
{
	vec3 lightDir = normalize(ub.PointLights_Position[lIndex].xyz - fs_in.Position.xyz); // lightDir : fragment -> light
	vec3 reflectDir = reflect(-lightDir, normal);
	
	// Ambient
	vec3 ambient = ub.PointLights_Color[lIndex].rgb * ub.PointLights_Intensity[lIndex].x;
	
	// Diffuse
	float diffuseFactor = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diffuseFactor * ub.PointLights_Color[lIndex].rgb * ub.PointLights_Intensity[lIndex].y;
	
	// Specular
	float specularFactor = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
	vec3 specular = specularFactor * ub.PointLights_Color[lIndex].rgb * ub.PointLights_Intensity[lIndex].z;
	
	// Attenuation
	float dist = distance(ub.PointLights_Position[lIndex].xyz, fs_in.Position.xyz);
	float attenuation = 1.0 / (ub.PointLights_Attenuation[lIndex].x + ub.PointLights_Attenuation[lIndex].y * dist + ub.PointLights_Attenuation[lIndex].z * dist * dist);
	
	// Mixing
	vec3 finalColor = (ambient + diffuse + specular) * attenuation;
	
	return finalColor;
}

vec3 CalculateNormal()
{
	vec3 normal = texture(Material_Normal, fs_in.TexCoord).rgb;
	normal = normal * 2.0 - 1.0;
	normal = normalize(fs_in.TBN * normal);
	return normal;
}

void main()
{
	vec3 normal = CalculateNormal();
	vec3 viewDir = normalize(ub.Camera_Position.xyz - fs_in.Position.xyz); // viewDir : fragment -> camera
	
	// Calculate Base color : Albedo
	vec3 albedo = texture(Material_Albedo, fs_in.TexCoord).rgb;
	
	vec3 finalColor = albedo;
	
	float roughness = texture(Material_Roughness, fs_in.TexCoord).r;
	shininess = 2.0/pow(roughness, 4) - 2.0;
	
	// Lighting Calculations
	vec3 lightingComponent = vec3(0.0);
	for (int i=0; i<MAX_DIR_LIGHTS; i++)
		lightingComponent += CalculateDirectionalLightComponent(normal, viewDir, i);
		
	for (int i=0; i<MAX_POINT_LIGHTS; i++)
		lightingComponent += CalculatePointLightComponent(normal, viewDir, i);
	
	finalColor *= lightingComponent;
	
	// Output to framebuffer target
	o_Color = vec4(finalColor, 1.0);
}