// Van der Corput Radical Inverse
float RadicalInverseVdC(uint bits) {
     bits = (bits << 16u) | (bits >> 16u);
     bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
     bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
     bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
     bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
     return float(bits) * 2.3283064365386963e-10; // / 0x100000000
}

vec2 Hammersley2D(uint i, uint N) {
     return vec2(float(i)/float(N), RadicalInverseVdC(i));
}

vec4 HemisphereSample_uniform(float u, float v) {
     float phi = v * 2.0 * PI;
     float cosTheta = 1.0 - u;
     float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
     //return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
     return vec4(sin(phi), cos(phi), sinTheta, cosTheta);
}
    
vec4 HemisphereSample_cos(float u, float v) {
    float phi = v * 2.0 * PI;
    float cosTheta = sqrt(1.0 - u);
    float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    //return vec3(cos(phi) * sinTheta, sin(phi) * sinTheta, cosTheta);
    return vec4(sin(phi), cos(phi), sinTheta, cosTheta);
}

vec4 ImportanceSample_GGX(in vec2 Xi, in vec3 N, float roughness)
{
	float a = roughness * roughness;

	float phi = Xi.x * 2.0 * PI;
	float cosTheta = sqrt((1.0 - Xi.y) / (1.0 + (a * a - 1.0) * Xi.y));
	float sinTheta = sqrt(1.0 - cosTheta * cosTheta);

	return vec4(sin(phi), cos(phi), sinTheta, cosTheta);
}
