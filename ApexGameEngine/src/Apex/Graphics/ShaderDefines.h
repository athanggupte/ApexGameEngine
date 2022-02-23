#pragma once

// Vertex Attribute locations
#define ATTRIB_LOC_Position		0
#define ATTRIB_LOC_UV0			1
#define ATTRIB_LOC_Color		2
#define ATTRIB_LOC_TexIndex		3
#define ATTRIB_LOC_Normal		4
#define ATTRIB_LOC_Tangent		5
#define ATTRIB_LOC_Bitangent	6
#define ATTRIB_LOC_UV1			7
#define ATTRIB_LOC_UserDefined	8

// Global Uniform Block bindings
#define UBO_BIND_Camera			0
#define UBO_BIND_Lights			1

// Global Uniform Sampler bindings
// #define TEX_SLOT_error			0
#define TEX_SLOT_ENV_Skybox		0
#define TEX_SLOT_MAT_Albedo		1
#define TEX_SLOT_MAT_Metallic	2
#define TEX_SLOT_MAT_Roughness	3
#define TEX_SLOT_MAT_Normal		4
// ... More texture slots to be used/reused

// User defined textures should use the following reserved texture slots (may be extended in the future)
#define TEX_SLOT_MAT_User0		8
#define TEX_SLOT_MAT_User1		9
#define TEX_SLOT_MAT_User2		10
#define TEX_SLOT_MAT_User3		11
#define TEX_SLOT_MAT_User4		12
#define TEX_SLOT_MAT_User5		13
#define TEX_SLOT_MAT_User6		14
#define TEX_SLOT_MAT_User7		15

// Global shader constants
// Forward Renderer
#define RFWD_MAX_DIRECTIONAL_LIGHTS	4
#define RFWD_MAX_POINT_LIGHTS		8
#define RFWD_MAX_SPOT_LIGHTS		8

// Forward+ Renderer
#define RFP_MAX_DIRECTIONAL_LIGHTS	4
#define RFP_MAX_POINT_LIGHTS		1024
#define RFP_MAX_SPOT_LIGHTS			512

#define MAX_DIRECTIONAL_LIGHTS	RFWD_MAX_DIRECTIONAL_LIGHTS
#define MAX_POINT_LIGHTS		RFWD_MAX_POINT_LIGHTS
#define MAX_SPOT_LIGHTS			RFWD_MAX_SPOT_LIGHTS
