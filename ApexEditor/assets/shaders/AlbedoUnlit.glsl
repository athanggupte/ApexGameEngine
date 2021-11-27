#type vertex
#version 450 core

#include <internal_assets/shaders/ShaderDefines.h>

layout(location = ATTRIB_LOC_Position) in vec3 a_Position;
layout(location = ATTRIB_LOC_UV0) in vec2 a_TexCoord;
layout(location = ATTRIB_LOC_Normal) in vec3 a_Normal;

layout(location = 0) uniform Camera {
	mat4 u_ViewProjection;
};
layout(location = 1) uniform mat4 u_Model;

layout(location = 0) out vec2 v_TexCoord;
layout(location = 1) out vec3 v_Normal;

void main()
{
	v_TexCoord = a_TexCoord;
	v_Normal = mat3(u_Model) * a_Normal;

	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec2 v_TexCoord;
layout(location = 1) in vec3 v_Normal;

layout(location = 2, binding = 0) uniform sampler2D Albedo;
layout(location = 3, binding = 1) uniform sampler2D Metallic;

void main()
{
	vec4 metallic = vec4(texture(Metallic, v_TexCoord).rgb, 1.0);
	// o_Color = texture(Albedo, v_TexCoord).rgba /* metallic*/;
	o_Color = vec4(v_Normal, 1.0);
}