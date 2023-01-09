#ifndef _APEX_INCLUDE_GLOBALS3D_GLH_
#define _APEX_INCLUDE_GLOBALS3D_GLH_

layout(std140, binding = UBO_BIND_Camera) uniform CameraUniforms
{
	vec4 Position;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	mat4 ViewProjectionMatrix;
	mat4 InverseViewMatrix;
}
u_Camera;

struct DirectionalLight
{
	vec4 Direction;
	vec4 Color;
};

struct PointLight
{
	vec4 Position;
	vec4 Color;
	float Radius;
	float AttConstant;
	float AttLinear;
	float AttQuadratic;
};

struct SpotLight
{
	vec4 Position;
	vec4 Direction;
	vec4 Color;
	float Radius;
	float InnerCutoff; // cos(innerCutoffAngle)
	float OuterCutoff; // cos(outerCutoffAngle)
	//float _padding;
};

layout(std140, binding = UBO_BIND_Lights) uniform Lights
{
	DirectionalLight DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
	PointLight PointLights[MAX_POINT_LIGHTS];
	SpotLight SpotLights[MAX_SPOT_LIGHTS];

	float NumDirectionalLights;
	float NumPointLights;
	float NumSpotLights;
}
u_Lights;

#define PI 3.1415926538
#define TwoPI 6.2831853076
#define HalfPI 1.5707963269
#define PI2 9.8696044024

#define EPSILON 1e-7

#endif // _APEX_INCLUDE_GLOBALS3D_GLH_
