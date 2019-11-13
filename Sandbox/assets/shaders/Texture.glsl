#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec3 v_Position;
out vec2 v_TexCoord;

void main()
{
	v_Position = a_Position;
	v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y * -1.0);
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

uniform sampler2D u_TextureDiffuse;

in vec3 v_Position;
in vec2 v_TexCoord;

void main()
{
	o_Color = texture(u_TextureDiffuse, v_TexCoord);
	//o_Color = vec4(v_TexCoord, 0.0, 1.0);
}
