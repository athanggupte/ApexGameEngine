#type vertex
#version 450 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) uniform Camera {
	mat4 u_ViewProjection;
};
layout(location = 1) uniform mat4 u_Model;

layout(location = 0) out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;

	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;
layout(location = 0) in vec2 v_TexCoord;

layout(location = 2, binding = 0) uniform sampler2D Albedo;
layout(location = 3, binding = 1) uniform sampler2D Metallic;

void main()
{
	vec4 metallic = vec4(texture(Metallic, v_TexCoord).rgb, 1.0);
	o_Color = texture(Albedo, v_TexCoord).rgba /* metallic*/;
}