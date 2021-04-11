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

layout(location = 0) out vec4 o_BrightColor;

in vec2 v_TexCoord;

uniform sampler2D u_ScreenTexture;

void main()
{
	vec4 screenColor = texture(u_ScreenTexture, v_TexCoord);
	float brightness = dot(screenColor.rgb, vec3(0.2126, 0.7152, 0.0722));
	if(brightness > 1.5)
		o_BrightColor = vec4(screenColor.rgb, 1.0);
	else
		o_BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}