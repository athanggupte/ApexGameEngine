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

uniform sampler2D u_SceneTexture;
uniform sampler2D u_BlurTexture;
uniform float exposure;

in vec2 v_TexCoord;

void main()
{
	vec3 sceneColor = texture(u_SceneTexture, v_TexCoord).rgb;
	vec3 blurColor = texture(u_BlurTexture, v_TexCoord).rgb;
	sceneColor += blurColor;

	vec3 result = vec3(1.0) - exp(-sceneColor * exposure);

	result = pow(result, vec3(1.0 / 2.2));
	o_Color = vec4(result, 1.0);
}
