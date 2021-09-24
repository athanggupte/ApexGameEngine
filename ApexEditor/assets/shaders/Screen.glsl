#type vertex
#version 450

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;
	gl_Position = vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

uniform sampler2D u_Texture;

in vec2 v_TexCoord;

void main()
{
	o_Color = vec4(vec3(texture(u_Texture, v_TexCoord).rgb), 1.0);
	//o_Color = vec4(0.83, 0.28, 0.23, 1.0);
}
