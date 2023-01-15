#type vertex
#version 450 core

layout(location = ATTRIB_LOC_Position)	in vec3 a_Position;
layout(location = ATTRIB_LOC_Normal)	in vec3 a_Normal;

layout(location = 0) out vec3 v_Position;
layout(location = 1) out vec3 v_Normal;

#include <internal_assets/shaders/Globals3D.glsl>


layout(location = 1) uniform mat4 u_Model;

void main()
{
	vec3 N = vec3(u_Model * vec4(a_Normal, 0.0));
	v_Normal = N;

	vec4 position_WorldSpace = u_Model * vec4(a_Position, 1.0);
	vec4 position_ViewSpace = u_Camera.ViewProjectionMatrix * position_WorldSpace;
	
	v_Position = position_WorldSpace.xyz;

	gl_Position = position_ViewSpace;
}


#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_Position;
layout(location = 1) in vec3 v_Normal;

#include <internal_assets/shaders/Globals3D.glsl>

layout(binding = TEX_SLOT_ENV_Skybox) uniform samplerCube _EnvSkybox;

void main()
{
	vec3 N = normalize(v_Normal);
	vec3 V = normalize(u_Camera.Position.xyz - v_Position);
	vec3 R = reflect(-V, N);
	
	vec3 color = texture(_EnvSkybox, R).rgb;

	o_Color = vec4(color.rgb, 1.0);
}