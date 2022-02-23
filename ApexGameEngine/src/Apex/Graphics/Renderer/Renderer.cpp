#include "apex_pch.h"
#include "Renderer.h"

#include "RenderCommands.h"
#include "Apex/Core/Camera.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Utils/Random.h"
//#include "Apex/Graphics/Model/Model.h"

#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Components.h"

#include <glm/gtc/random.hpp>

namespace Apex {

	struct __CameraUBOData
	{
		glm::vec4 Position;
		glm::mat4 ViewMatrix;
		glm::mat4 ProjectionMatrix;
		glm::mat4 ViewProjectionMatrix;
		glm::mat4 InverseViewMatrix;
	};

	struct __DirectionalLightUBOData
	{
		glm::vec4 Direction;
		glm::vec4 Color;
	};

	struct __PointLightUBOData
	{
		glm::vec4 Position;
		glm::vec4 Color;
		float Radius;
		float AttConstant;
		float AttLinear;
		float AttQuadratic;
	};

	struct __LightsUBOData
	{
		__DirectionalLightUBOData DirectionalLights[MAX_DIRECTIONAL_LIGHTS];
		__PointLightUBOData PointLights[MAX_POINT_LIGHTS];

		float NumDirectionalLights;
		float NumPointLights;

		//glm::vec2 _padding;
	};

	constexpr size_t CAMERA_UBO_SIZE = sizeof(__CameraUBOData);
	constexpr size_t LIGHTS_UBO_SIZE = sizeof(__LightsUBOData) + (sizeof(__LightsUBOData) % 16);

	struct SceneData
	{
		//glm::mat4 ViewProjectionMatrix;
		//glm::mat4 ViewMatrix;
		//glm::mat4 InverseViewMatrix;
		bool ImageAccess = false;

		Ref<UniformBuffer> cameraUniformBuffer = UniformBuffer::Create(CAMERA_UBO_SIZE, UBO_BIND_Camera);
		Ref<UniformBuffer> lightUniformBuffer = UniformBuffer::Create(LIGHTS_UBO_SIZE, UBO_BIND_Lights);
	};

	static SceneData* s_SceneData;

	void Renderer::Init()
	{
		RenderCommands::Init();
		s_SceneData = new SceneData;
		APEX_CORE_TRACE("Apex::Renderer initialized successfully!");
	}

	void Renderer::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		//s_SceneData->InverseViewMatrix = transform;
		//s_SceneData->ViewMatrix = glm::inverse(transform);
		//s_SceneData->ViewProjectionMatrix = camera.GetProjection() * s_SceneData->ViewMatrix;
		auto projMat = camera.GetProjection();
		auto viewMat = glm::inverse(transform);
		__CameraUBOData cameraBuffer {
			transform[3],
			viewMat,
			projMat,
			projMat * viewMat,
			transform
		};
		s_SceneData->cameraUniformBuffer->SetData(&cameraBuffer, sizeof(__CameraUBOData));
		s_SceneData->cameraUniformBuffer->Bind();
	}

	void Renderer::EndScene()
	{
		if (s_SceneData->ImageAccess)
			RenderCommands::ImageAccessBarrier();
	}

	void Renderer::SetupLights(const Ref<Scene>& scene)
	{
		const auto bufferPtr = static_cast<__LightsUBOData*>(s_SceneData->lightUniformBuffer->MapBuffer(false));

		int directionalLightsCount = 0;
		int pointLightsCount = 0;

		//int numDirectionalLights = std::min(directionalLightsCount, MAX_DIRECTIONAL_LIGHTS);
		//int numPointLights = std::min(pointLightsCount, MAX_POINT_LIGHTS);

		scene->View<TransformComponent, LightComponent>()
		.each(
			[bufferPtr, &directionalLightsCount, &pointLightsCount]
			(TransformComponent& transform_component, LightComponent& light_component)
			{
				switch (light_component.type)
				{
				case LightType::DirectionalLight:
					{
						if (directionalLightsCount < MAX_DIRECTIONAL_LIGHTS) {
							auto& light = bufferPtr->DirectionalLights[directionalLightsCount];
							light.Color = light_component.color * light_component.intensity;
							light.Direction = glm::vec4{ glm::rotate(glm::quat(transform_component.rotation), glm::vec3{ 0.f, -1.f, 0.f }), 1.f };

							++directionalLightsCount;
						}
						break;
					}
				case LightType::PointLight:
					{
						if (pointLightsCount < MAX_POINT_LIGHTS) {
							auto& light = bufferPtr->PointLights[pointLightsCount];
							light.Color = light_component.color * light_component.intensity;
							light.Position = glm::vec4{ transform_component.translation, 1.f };
							light.AttConstant = light_component.attenuationConstant;
							light.AttLinear = light_component.attenuationLinear;
							light.AttQuadratic = light_component.attenuationQuadratic;
							light.Radius = light_component.radius;

							++pointLightsCount;
						}
						break;
					}
				case LightType::SpotLight:
					{
						break;
					}
				case LightType::AreaLight:
					{
						break;
					}
				default: break;
				}
			}
		);

		/*for (int i = 0; i < numDirectionalLights; i++) {
			__DirectionalLightUBOData directionalLight {
				glm::vec4{ glm::sphericalRand(1.f), 1.f },
				glm::vec4{ 9.f, 8.f, 5.f, 1.f }
			};
			bufferPtr->DirectionalLights[i] = directionalLight;
		}
		
		for (int i = 0; i < numPointLights; i++) {
			__PointLightUBOData pointLight {
				glm::vec4{ glm::vec3{ static_cast<float>(i) }, 1.f },
				glm::vec4{ 10.f, 10.f, 10.f, 10.f },
				10.f,
				0.f, 0.f, 0.f
			};
			bufferPtr->PointLights[i] = pointLight;
		}*/

		bufferPtr->NumDirectionalLights = static_cast<float>(directionalLightsCount);
		bufferPtr->NumPointLights = static_cast<float>(pointLightsCount);

		s_SceneData->lightUniformBuffer->UnmapBuffer();
		s_SceneData->lightUniformBuffer->Bind();
	}

	void Renderer::Submit(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, const glm::mat4& modelMatrix)
	{
		shader->Bind();
		//shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		//shader->SetUniMat4("u_NormalMat", glm::transpose(glm::inverse(s_SceneData->ViewMatrix * modelMatrix)));
		shader->SetUniMat4("u_Model", modelMatrix);

		vertexArray->Bind();
		if (vertexArray->GetIndexBuffers().empty())
			RenderCommands::Draw(vertexArray);
		else
			RenderCommands::DrawIndexed(vertexArray);
	}

	void Renderer::SubmitArray(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray, glm::mat4 modelMatrices[], size_t count)
	{
		shader->Bind();
		//shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
		shader->SetUniMat4v("u_Model", modelMatrices, count);

		vertexArray->Bind();
		RenderCommands::DrawInstanced(vertexArray, static_cast<uint32_t>(count));
	}

// 	void Renderer::SubmitModel(const Ref<Shader>& shader, const Ref<Model>& model, const glm::mat4 & modelMatrix)
// 	{
// 		model->ApplyModelMatrix(modelMatrix);
// 		shader->Bind();
// 		shader->SetUniMat4("u_ViewProjection", s_SceneData->ViewProjectionMatrix);
// 		shader->SetUniMat4("u_Model", model->GetModelMatrix());
// 
// 		for (auto&[name, mesh] : model->GetMeshes()) {
// 			if (mesh->Show()) {
// 				uint32_t i = 1;
// 				APEX_CORE_INFO("Mesh textures ->");
// 				for (auto[name, texture] : mesh->GetTextures()) {
// 					APEX_CORE_INFO("{0} : {1} : {2}", name, texture->GetPath(), i);
// 					shader->SetUniInt1("u_" + name, i);
// 					texture->Bind(i);
// 					i++;
// 				}
// 				mesh->GetVAO()->Bind();
// 				RenderCommands::DrawIndexed(mesh->GetVAO());
// 			}
// 		}
// 	}
	
// #if 0
	/*--------------------------------------------------------------------------------------------------*/
	void Renderer::SubmitPostProcess(const Ref<Shader>& shader, const Ref<VertexArray>& vertexArray)
	{
		shader->Bind();
		vertexArray->Bind();
		RenderCommands::Draw(vertexArray);
	}

	void Renderer::SetImageAccessBit()
	{
		s_SceneData->ImageAccess = true;
	}

	/*--------------------------------------------------------------------------------------------------*/
// #endif

	void Renderer::Shutdown()
	{
		delete s_SceneData;
	}
	
}
