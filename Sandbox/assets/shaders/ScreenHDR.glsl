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
uniform float exposure;
uniform float contrast;

in vec2 v_TexCoord;

void main()
{
	vec3 hdrColor = texture(u_Texture, v_TexCoord).rgb;
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

	result = pow(result, vec3(1.0 / 2.2));
	o_Color = vec4(result, 1.0);

	o_Color.rgb = (o_Color.rgb - 0.5) * (1.0 + contrast) + 0.5;
}
