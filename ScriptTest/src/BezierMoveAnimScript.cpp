#include "pch.h"

#include "Apex/Core/ECS/EntityScript.h"
#include "Apex/Core/ECS/ScriptFactory.h"
#include "Apex/Core/ECS/Components/BasicComponents.h"
#include "Apex/Graphics/Renderer/RenderCommands.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"

using namespace Apex;

class SCRIPT_TEST_API BezierMoveAnimScript : public EntityScript
{
public:
	float Speed = 2.f;
	glm::vec3 Points[4]
	{
		{ 0.f, 0.f, 0.f },
		{ 15.f, 6.f, -12.f },
		{ -15.f, 12.f, -12.f },
		{ 0.f, 18.f, 0.f },
	};

protected:
	const size_t NumSegments = 100u;
	
	std::vector<float> TtoDLUT;
	std::vector<float> DtoTLUT;
	float TravelledDistance = 0.f;

	Ref<Shader> BezierShader;
	Ref<VertexArray> BezierVAO;

	void OnCreate() override
	{
		std::vector<glm::vec3> coords(NumSegments + 1);
		TtoDLUT.resize(NumSegments + 1);
		DtoTLUT.resize(NumSegments + 1);
		glm::vec3 coord = GetPointOnBezierCurve(0);
		coords[0] = coord;
		TtoDLUT[0] = 0;
		auto step = 1.f / (float)NumSegments;
		auto lenAccumulator = 0.f;
		for (int i = 1; i <= NumSegments; i++) {
			auto nextCoord = GetPointOnBezierCurve(step * (float)i);
			lenAccumulator += glm::distance(nextCoord, coord);
			TtoDLUT[i] = lenAccumulator;
			coord = nextCoord;
			//coords[i] = coord;
		}

		DtoTLUT[0] = 0;
		auto dstep = lenAccumulator / (float)NumSegments;
		auto acc = 0.f;
		for (int i = 1; i <= NumSegments; i++) {
			acc += dstep;
			auto t = GetTFromDistance(acc);
			DtoTLUT[i] = t;
			coords[i] = GetPointOnBezierCurve(t);
		}

		auto bezierVbo = VertexBuffer::Create(&coords[0].x, coords.size() * sizeof(glm::vec3));
		bezierVbo->SetLayout({
			{ ShaderDataType::Float3, VertexElementType::Position }
		});

		BezierVAO = VertexArray::Create();
		BezierVAO->AddVertexBuffer(bezierVbo);

		const auto vertexSrc = R"(
		#version 450

		layout(location = ATTRIB_LOC_Position)	in vec3 a_Position;

		layout(std140, binding = UBO_BIND_Camera) uniform CameraUniforms
		{
			vec4 Position;
			mat4 ViewMatrix;
			mat4 ProjectionMatrix;
			mat4 ViewProjectionMatrix;
			mat4 InverseViewMatrix;
		} u_Camera;
		layout(location = 1) uniform mat4 u_Model;

		void main()
		{
			vec4 position_WorldSpace = vec4(a_Position, 1.0);
			vec4 position_ViewSpace = u_Camera.ViewProjectionMatrix * position_WorldSpace;

			gl_Position = position_ViewSpace;
		}
		)";

		const auto fragmentSrc = R"(
		#version 450

		layout(location = 0) out vec4 o_Color;

		void main()
		{
			o_Color = vec4(1.0, 1.0, 0.0, 1.0);
		}
		)";

		BezierShader = Shader::Create("Bezier", vertexSrc,fragmentSrc);
	}

	void OnUpdate(Timestep ts) override
	{
		TravelledDistance += ts.GetSeconds() * Speed;
		if (TravelledDistance <= TtoDLUT.back()) {
			auto& transform = self().Transform();
			transform.translation = GetPointOnBezierCurve(GetTFromDistance(TravelledDistance));
		}
	}

	void OnRender(Timestep ts) override
	{
		BezierShader->Bind();
		//Apex::RenderCommands::Draw(BezierVAO, DrawMode::LINE_STRIP);
		RenderCommands::SetPointSize(4.f);
		RenderCommands::Draw(BezierVAO, DrawMode::POINTS);
	}

	glm::vec3 GetPointOnBezierCurve(float t)
	{
		float t2 = t * t;

		float omt = 1 - t;
		float omt2 = omt * omt;

		auto p = omt2 * omt * Points[0]
		                            + 3.f * omt2 * t * Points[1]
		                            + 3.f * t2 * omt * Points[2]
		                            + t2 * t * Points[3];
		return p;
	}

	float GetTFromDistance(float distance)
	{
		auto maxDist = TtoDLUT.back();
		if (distance < 0.f || distance > maxDist - 1.f)
			return distance / maxDist;

		auto it = std::lower_bound(TtoDLUT.begin(), TtoDLUT.end(), distance);
		auto idx = it - TtoDLUT.begin();
		auto lerpFactor = (distance - *it) / (*(it+1) - *it);
		return glm::mix((float)idx, (float)idx + 1, lerpFactor) / (float)TtoDLUT.size();
	}
};

APEX_REGISTER_SCRIPT_FACTORY(BezierMoveAnimScript)
