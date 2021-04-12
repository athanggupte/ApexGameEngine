#type vertex
#version 450

layout(location = 0) in vec2 a_Position;

void main()
{
	gl_Position = vec4(a_Position, 0.0, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(0.78, 0.54, 0.23, 1.0);
}
 
