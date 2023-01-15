#type vertex
#version 450 core

#include "Globals3D.glsl"

layout(location = 0) out vec3 v_ViewDir;

const vec3 pos[4] = vec3[4](
	vec3(-1.0, -1.0, 0.0),
	vec3( 1.0, -1.0, 0.0),
	vec3( 1.0,  1.0, 0.0),
	vec3(-1.0,  1.0, 0.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

void main()
{
	int idx = indices[gl_VertexID];
	vec3 position = pos[idx];
	
	vec2 multiply = vec2(0, 0);
	multiply.x = 1.0 / u_Camera.ProjectionMatrix[0][0];
	multiply.y = 1.0 / u_Camera.ProjectionMatrix[1][1];
	
	vec3 tempPos = vec3(position.xy * multiply.xy, -1.0);
	v_ViewDir = transpose(mat3(u_Camera.ViewMatrix)) * normalize(tempPos);

	gl_Position = vec4(position.xy, 1.0, 1.0);
}

#type fragment
#version 450 core

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_ViewDir;

layout(binding = TEX_SLOT_ENV_Skybox) uniform samplerCube u_Skybox;

void main()
{
	o_Color = texture(u_Skybox, normalize(v_ViewDir));
}
