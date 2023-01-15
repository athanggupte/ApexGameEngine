#type vertex
#version 450 core

layout(location = ATTRIB_LOC_Position)	in vec3 a_Position;
layout(location = ATTRIB_LOC_UV0)		in vec2 a_TexCoord;

layout(location = 0) out vec2 v_TexCoord;

#include "Globals3D.glsl"

layout(location = 1) uniform mat4 u_Model;

void main()
{
	v_TexCoord = a_TexCoord;
	
	vec4 position_WorldSpace = u_Model * vec4(a_Position, 1.0);
	vec4 position_ViewSpace = u_Camera.ViewProjectionMatrix * position_WorldSpace;
	
	gl_Position = position_ViewSpace;
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;

layout(binding = TEX_SLOT_MAT_Albedo) uniform sampler2D _MatAlbedo;

void main()
{
	o_Color = texture(_MatAlbedo, v_TexCoord).rgba /* metallic*/;
}