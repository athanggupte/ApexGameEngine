#include <apex_pch.h>
#include "ShaderNodes.h"

#include "ShaderSlots.h"

namespace Apex::EditorTools {

	/* OutputNode */
	OutputNode::OutputNode()
	{
		name = "Output";
		inputSlots = {
			new EditorTools::SlotVec4({ "Color" })
		};
		outputSlots = {};
		size = { 6.f, 0.f };
	}

	std::string OutputNode::GetFunction()
	{
		return "_outColor_ = Res##";
	}




	/* TestNode */
	TestNode::TestNode()
	{
		name = "Test";
		inputSlots = {
			new EditorTools::SlotFloat({ "FloatInput" }),
			new EditorTools::SlotVec2({ "Vec2Input" }),
			new EditorTools::SlotVec3({ "Vec3Input" }),
			new EditorTools::SlotVec4({ "Vec4Input" })
		};
		outputSlots = {
			new EditorTools::SlotFloat({ "FloatOutput" }),
			new EditorTools::SlotVec2({ "Vec2Output" }),
			new EditorTools::SlotVec3({ "Vec3Output" }),
			new EditorTools::SlotVec4({ "Vec4Output" })
		};
		size = { 24.f, 0.f };
	}

	std::string TestNode::GetFunction()
	{
		// void Test(in float in1, in vec2 in2, in vec3 in3, in vec4 in4, out float out1, out vec2 out2, out vec3 out3, out vec4 out4);
		return "Test(FloatInput, Vec2Input, Vec3Input, Vec4Input, FloatOutput, Vec2Output, Vec3Output, Vec4Output)";
	}


	/* BlurNode */
	BlurNode::BlurNode()
	{
		name = "Blur";
		inputSlots = {
			new EditorTools::SlotTexture2D({ "TexIn" })
		};
		outputSlots = {
			new EditorTools::SlotVec4({ "Out" })
		};
		size = { 24.f, 0.f };

		previewCompute = ComputeShader::Create("blur",
			"#version 450\n"
			"layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;\n"
			"layout(binding = 0) uniform writeonly image2D outImage;\n"
			"layout(rgba16f, binding = 0) uniform readonly image2D outImage;\n"
			"layout(rgba16f, binding = 0) uniform readonly image2D outImage;\n"
			"void main() {\n"
			"ivec2 pCoords = ivec2(gl_GlobalInvocationID.xy);\n"
			"\n"
		);
	}

	TextureNode::TextureNode()
	{
	}

}