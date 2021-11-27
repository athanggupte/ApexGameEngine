#type vertex
#version 450

uniform mat4 u_ViewProjection;
uniform mat4 u_ModelMatrix;

const float gridSize = 1000.0;

const vec3 pos[4] = vec3[4](
	vec3(-1.0, 0.0, -1.0),
	vec3( 1.0, 0.0, -1.0),
	vec3( 1.0, 0.0,  1.0),
	vec3(-1.0, 0.0,  1.0)
);

const int indices[6] = int[6](
	0, 1, 2, 2, 3, 0
);

layout(location = 0) out vec2 v_UV;

void main()
{
	int idx = indices[gl_VertexID];
	vec4 position = vec4(u_ModelMatrix * vec4(pos[idx] * gridSize, 1.0));

	v_UV = position.xz;

	gl_Position = u_ViewProjection * vec4(position);
}

#type fragment
#version 450

layout(location = 0) in vec2 v_UV;
layout(location = 0) out vec4 o_Color;

const float gridSize = 1000.0;
const float gridCellSize = 1.0;

const vec4 gridColorThin = vec4(0.5, 0.5, 0.5, 1.0);
const vec4 gridColorThick = vec4(1.0, 1.0, 1.0, 1.0);

const float gridMinPixelsBetweenCells = 4.0;

float log10(float x)
{
	return log(x) / log(10.0);
}

float satf(float x)
{
	return clamp(x, 0.0, 1.0);
}

vec2 satv(vec2 x)
{
	return clamp(x, vec2(0.0), vec2(1.0));
}

float max2(vec2 v)
{
	return max(v.x, v.y);
}

vec4 gridColor(vec2 uv)
{
	vec2 dudv = vec2(
		length(vec2(dFdx(uv.x), dFdy(uv.x))),
		length(vec2(dFdx(uv.y), dFdy(uv.y)))
	);

	float lodLevel = max(0.0, log10((length(dudv) * gridMinPixelsBetweenCells) / gridCellSize) + 1.0);
	float lodFade = fract(lodLevel);

	// cell sizes for lod0, lod1 and lod2
	float lod0 = gridCellSize * pow(10.0, floor(lodLevel));
	float lod1 = lod0 * 10.0;
	float lod2 = lod1 * 10.0;

	// each anti-aliased line covers up to 4 pixels
	dudv *= 2.0;

	// calculate absolute distances to cell line centers for each lod and pick max X/Y to get coverage alpha value
	float lod0a = max2( vec2(1.0) - abs(satv(mod(uv, lod0) / dudv) * 2.0 - vec2(1.0)) );
	float lod1a = max2( vec2(1.0) - abs(satv(mod(uv, lod1) / dudv) * 2.0 - vec2(1.0)) );
	float lod2a = max2( vec2(1.0) - abs(satv(mod(uv, lod2) / dudv) * 2.0 - vec2(1.0)) );

	// blend between falloff colors to handle LOD transition
	vec4 c = lod2a > 0.0 ? gridColorThick : lod1a > 0.0 ? mix(gridColorThick, gridColorThin, lodFade) : gridColorThin;

	// calculate opacity falloff based on distance to grid extents
	float opacityFalloff = (1.0 - satf(length(uv) / gridSize));
	// float opacityFalloff = 1.0;

	// blend between LOD level alphas and scale with opacity falloff
	c.a *= (lod2a > 0.0 ? lod2a : lod1a > 0.0 ? lod1a : (lod0a * (1.0-lodFade))) * opacityFalloff;

	return c;
}

void main()
{
	vec4 grid = gridColor(v_UV);
	if (grid.a < 0.1)
		discard;
	o_Color = gridColor(v_UV);
	// o_Color = vec4(v_UV, 0.0, 1.0);
}
