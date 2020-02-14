#type vertex
#version 450

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat4 u_LightSpace;

out vec3 v_Position;
out vec2 v_TexCoord;
out vec4 v_LightSpacePosition;

void main()
{
	v_Position = a_Position;
	v_TexCoord = vec2(a_TexCoord.x, a_TexCoord.y * -1.0);
	v_LightSpacePosition = u_LightSpace * vec4(v_Position, 1.0);
	gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 450

layout(location = 0) out vec4 o_Color;

uniform sampler2D u_TextureDiffuse;
uniform sampler2D u_TextureLightDepth;

in vec3 v_Position;
in vec2 v_TexCoord;
in vec4 v_LightSpacePosition;

uniform vec3 u_LightPositions[2];
uniform vec3 u_LightColors[2];

float ShadowCalculation(vec3 normal, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = v_LightSpacePosition.xyz / v_LightSpacePosition.w;
    
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(u_TextureLightDepth, projCoords.xy).r; 
    
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	
	if(currentDepth > 1.0)
		return 0.0;
    
	float bias = max(0.01 * (1.0 - dot(normal, lightDir)), 0.005);  
	//float bias = 0.005;

	// check whether current frag pos is in shadow
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	//float shadow = currentDepth > closestDepth  ? 1.0 : 0.0;

    return shadow;
}  

void main()
{
	vec3 L = normalize(u_LightPositions[0] - v_Position);

	float shadow = ShadowCalculation(vec3(0.0, 1.0, 0.0), L);

	vec3 albedo = texture(u_TextureDiffuse, v_TexCoord).rgb;

	vec3 color = vec3(max(1.0 - shadow, 0.3)) * albedo;

	o_Color = vec4(color, 1.0);
	//o_Color = vec4(v_TexCoord, 0.0, 1.0);
}
