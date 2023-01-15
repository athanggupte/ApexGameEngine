#type vertex
#version 450

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;

void main()
{	
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

float rand(float n)
{
    return fract(sin(n) * 43758.5453123);
}

vec3 getPrimitiveColor()
{
    int random = int(rand(gl_PrimitiveID+1) * 163443835);
    vec3 color;
    color.r = ((random / 53) % 256) / 255.0f;
    color.g = ((random / 13) % 256) / 255.0f;
    color.b = ((random / 17) % 256) / 255.0f;
    return color;
}

void main()
{
	o_Color = vec4(getPrimitiveColor(), 1.0);
}