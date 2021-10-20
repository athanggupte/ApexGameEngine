#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec4 v_Color;
out vec2 v_TexCoord;

void main()
{
	v_Color = a_Color;
	v_TexCoord = a_TexCoord;
	
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

layout(binding = 0) uniform sampler2D u_Texture;

in vec4 v_Color;
in vec2 v_TexCoord;

void main()
{
	o_Color = v_Color;
	// o_Color *= texture(u_Texture, v_TexCoord).rgba;
	// o_Color = vec4(1.0, 0.0, 0.0, 1.0);
}