#type vertex
#version 450

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord[11];

uniform int u_Horizontal;
uniform vec2 u_Resolution;

void main()
{
	gl_Position = vec4(a_Position, 0.0, 1.0);

	vec2 centerTexCoord = a_TexCoord;
	if(u_Horizontal == 1) {
		float pixelSize = 1.0 / u_Resolution.x;

		for(int i=-5; i<=5; i++) {
			v_TexCoord[i+5] = centerTexCoord + vec2(pixelSize * i, 0.0);
		}
	}
	else {
		float pixelSize = 1.0 / u_Resolution.y;

		for(int i=-5; i<=5; i++) {
			v_TexCoord[i+5] = centerTexCoord + vec2(0.0, pixelSize * i);
		}
	}

}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

in vec2 v_TexCoord[11];

uniform sampler2D u_ScreenTexture;
uniform int u_Horizontal;

void main()
{
	o_Color = vec4(0.0);
	o_Color += texture(u_ScreenTexture, v_TexCoord[0]) * 0.0093;
	o_Color += texture(u_ScreenTexture, v_TexCoord[1]) * 0.028002;
	o_Color += texture(u_ScreenTexture, v_TexCoord[2]) * 0.065984;
	o_Color += texture(u_ScreenTexture, v_TexCoord[3]) * 0.121703;
	o_Color += texture(u_ScreenTexture, v_TexCoord[4]) * 0.175713;
	o_Color += texture(u_ScreenTexture, v_TexCoord[5]) * 0.198596;
	o_Color += texture(u_ScreenTexture, v_TexCoord[6]) * 0.175713;
	o_Color += texture(u_ScreenTexture, v_TexCoord[7]) * 0.121703;
	o_Color += texture(u_ScreenTexture, v_TexCoord[8]) * 0.065984;
	o_Color += texture(u_ScreenTexture, v_TexCoord[9]) * 0.028002;
	o_Color += texture(u_ScreenTexture, v_TexCoord[10]) * 0.0093;
}