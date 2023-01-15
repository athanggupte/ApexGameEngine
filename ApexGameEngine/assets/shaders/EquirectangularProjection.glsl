#type vertex
#version 450

const vec3 pos[4] = vec3[4](
	vec3(-1.0, -1.0, -1.0),
	vec3( 1.0, -1.0, -1.0),
	vec3( 1.0,  1.0, -1.0),
	vec3(-1.0,  1.0, -1.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

#define X vec3(1.0, 0.0, 0.0)
#define Y vec3(0.0,-1.0, 0.0) // Inverted to account for inversion in cubemap texture
#define Z vec3(0.0, 0.0, 1.0)

mat3 view[6] = mat3[6](
	// view : ( Right, Up, Front )
	// Right
	mat3(  Z,  Y,  X ),
	// Left
	mat3( -Z,  Y, -X ),
	// Top
	mat3( -X, -Z,  Y ),
	// Bottom
	mat3( -X,  Z, -Y ),
	// Back
	mat3( -X,  Y,  Z ),
	// Front
	mat3(  X,  Y, -Z )
);

#undef X
#undef Y
#undef Z
#undef W

// Fixed perxpective project matrix
// FOV : 90deg
// aspect ratio : 1
// near : 0.1
// far  : 10
mat4 projection = mat4 (
	// ( 1 / (aspect * tan(90/2)), 0, 0, 0 )
	vec4( 1.0, 0.0, 0.0, 0.0 ),
	// ( 0, 1 / tan(90/2), 0, 0 )
	vec4( 0.0, 1.0, 0.0, 0.0 ),
	// ( 0, 0, (- near - far) / (near - far), 1 )
	vec4( 0.0, 0.0, -10.1 / -9.9, 1.0 ),
	// ( 0, 0, 2 * near * far / ( near - far ), 0 )
	vec4( 0.0, 0.0, 2 / -9.9, 0.0 )
);

layout(location = 0) out vec3 v_ViewDir;

layout(location = 0) uniform int u_FaceIndex;

void main()
{
	int vert_idx = indices[gl_VertexID];
	vec3 position = pos[vert_idx];
	
	v_ViewDir = transpose(view[u_FaceIndex]) * normalize(position);

	gl_Position = vec4(position.xy, 1.0, 1.0);
}


#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

layout(location = 0) in vec3 v_ViewDir;

layout(binding = 1) uniform sampler2D u_EquirectangularMap;

vec2 SampleSphericalMap(vec3 v)
{
	const vec2 invAtan = vec2(0.1591, 0.3183);
	vec2 uv = vec2(atan(v.z, v.x), asin(v.y));

	uv *= invAtan;
	uv += 0.5;
	return uv;
}

void main()
{
	vec3 viewDir = normalize(v_ViewDir);
	vec2 uv = SampleSphericalMap(viewDir);

	o_Color = vec4(texture(u_EquirectangularMap, uv).rgb, 1.0);
}
