#type vertex
#version 450

layout(location = ATTRIB_LOC_Position)	in vec3 a_Position;
layout(location = ATTRIB_LOC_Normal)	in vec3 a_Normal;

layout(location = 0) out vec3 v_Normal;

void main()
{
	//vec4 position_WorldSpace = u_Model * vec4(a_Position, 1.0);
	//vec4 position_ViewSpace = u_Camera.ViewProjectionMatrix * position_WorldSpace;
	
	gl_Position = vec4(a_Position, 1.0);
	
	//vec3 normal_WorldSpace = vec3(u_Model * vec4(a_Normal, 0.0));
	//vec4 normal_ViewSpace = normalize(u_Camera.ViewProjectionMatrix * vec4(normal_WorldSpace, 0.0));

	v_Normal = a_Normal;
}


#type geometry
#version 450

layout(triangles) in;
layout(line_strip, max_vertices = 6 + 4) out;

layout(location = 0) in vec3 v_Normal[];

#include "../Globals3D.glsl"

layout(location = 1) uniform mat4 u_Model;

#define NORMAL_LENGTH 0.2

void main()
{
	// Emit wireframe
	mat4 MVP = u_Camera.ViewProjectionMatrix * u_Model;
	for (int i = 0; i < gl_in.length(); i++) {
		vec3 P = gl_in[i].gl_Position.xyz;
		gl_Position = MVP * vec4(P, 1.0);
		EmitVertex();
	}
	vec3 P = gl_in[0].gl_Position.xyz;
	gl_Position = MVP * vec4(P, 1.0);
	EmitVertex();
	EndPrimitive();

	// Emit vertex normals
	for (int i = 0; i < gl_in.length(); i++) {
		vec3 P = gl_in[i].gl_Position.xyz;
		vec3 N = normalize(v_Normal[i].xyz);

		gl_Position = MVP * vec4(P, 1.0);
		EmitVertex();
		
		vec3 invscale = vec3(1.0);
		invscale.x = length(MVP[0]);
		invscale.y = length(MVP[1]);
		invscale.z = length(MVP[2]);
		invscale = 1.0 / invscale;

		gl_Position = MVP * vec4(P + invscale * N * NORMAL_LENGTH, 1.0);
		EmitVertex();

		EndPrimitive();
	}
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

void main()
{
	o_Color = vec4(1.0, 0.0, 0.0, 1.0);
}