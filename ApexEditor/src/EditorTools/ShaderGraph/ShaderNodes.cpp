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

}