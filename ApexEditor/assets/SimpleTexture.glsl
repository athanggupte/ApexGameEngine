#type vertex
#version 450

layout(location = 0) in vec2 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_TexCoord;

	gl_Position = u_Model * vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

layout(binding = 1) uniform writeonly image2D u_Image;
uniform sampler2D u_Texture;

in vec2 v_TexCoord;

void main()
{
	o_Color = texture(u_Texture, v_TexCoord).rgba;
	imageStore(u_Image, ivec2(gl_FragCoord.xy), vec4( gl_FragCoord.x, gl_FragCoord.y, 0.0, 1.0));
	memoryBarrierImage();
}