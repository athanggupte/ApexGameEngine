#include "apex_pch.h"
#include "ShadowMap.h"

#include "Apex/Core/Camera.h"
#include "Apex/Core/ECS/Entity.h"
#include "Apex/Core/ECS/Scene.h"
#include "Apex/Core/ECS/Components/BasicComponents.h"
#include "Apex/Core/ECS/Components/RendererComponents.h"

#include "glm/gtc/type_ptr.hpp"
#include "Renderer/RenderCommands.h"
#include "RenderPrimitives/Framebuffer.h"
#include "RenderPrimitives/Shader.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Apex {

	#define _MAX_CASCADE_LEVELS 5
	#define LIGHT_SPACE_MATRICES_UBO_BINDING 0

	static Ref<Shader> s_ShadowShader;
	static constexpr uint32_t MAX_CASCADE_LEVELS = _MAX_CASCADE_LEVELS;


	ShadowMap::ShadowMap(uint32_t width, uint32_t height, uint32_t cascades)
		: m_Framebuffer(Framebuffer::Create(FramebufferSpec{width, height, 0, false, false})),
		  m_LightMatricesUBO(UniformBuffer::Create(sizeof(glm::mat4) * cascades, LIGHT_SPACE_MATRICES_UBO_BINDING)), m_LightSpaceMatrices(cascades, glm::mat4{1.0}),
		  m_Cascades(std::max(std::min(cascades, MAX_CASCADE_LEVELS), 1u))
	{
		m_Framebuffer->AttachDepthTexture(Texture2DArray::Create(width, height, cascades, TextureSpec{
			                                                         TextureAccessFormat::DEPTH,
			                                                         TextureInternalFormat::DEPTH32,
			                                                         TextureDataType::FLOAT,
			                                                         TextureFiltering::NEAREST
		                                                         }));
		/*m_Framebuffer->AttachDepthTexture(Texture2D::Create(width, height, TextureSpec{
			                                                         TextureAccessFormat::DEPTH,
			                                                         TextureInternalFormat::DEPTH32,
			                                                         TextureDataType::FLOAT,
			                                                         TextureFiltering::NEAREST
		                                                         }));*/

		if (!s_ShadowShader)
		{
			// Vertex shader   : performs Model transformation only
			// Geometry shader : performs VP transformation to light space for each cascade and send it to separate layers
			// Fragment shader : only updates depth and does not do any other processing
			const std::string shaderSource = R"(
			#type vertex
			#version 450

			layout(location = ATTRIB_LOC_Position) in vec3 a_Position;

			// layout(location = 0) uniform mat4 u_LightVP;
			layout(location = 0) uniform mat4 u_Model;

			void main()
			{
				gl_Position = /*u_LightVP **/ u_Model * vec4(a_Position, 1.0);
			}

			#type geometry
			#version 450

			layout(triangles, invocations = 5) in;
			layout(triangle_strip, max_vertices = 3) out;

			layout(std140, binding = )" XSTR(LIGHT_SPACE_MATRICES_UBO_BINDING) R"() uniform LightSpaceMatrices
			{
				mat4 lightSpaceMatrices[)" XSTR(_MAX_CASCADE_LEVELS) R"(];
			};

			void main()
			{
				for (int i = 0; i < 3; i++) {
					gl_Position = lightSpaceMatrices[gl_InvocationID] * gl_in[i].gl_Position;
					gl_Layer = gl_InvocationID;
					EmitVertex();
				}
				EndPrimitive();
			}

			#type fragment
			#version 450

			void main()
			{
			}
			)";
			s_ShadowShader = Shader::Create("Shadow Mapping Shader", shaderSource);
		}
	}

	static glm::vec2 near_far_planes;

	static glm::mat4 CalculateLightViewProjectionMatrix(const glm::vec3& direction, const Camera& camera, const glm::mat4& camera_transform)
	{
		// 1. Find corners of Camera's view frustum
		glm::vec4 frustumCorners[8];
		const glm::mat4 inverse = camera_transform * glm::inverse(camera.GetProjection());

		for (int c = 0, x = 0; x < 2; x++) {
			for (int y = 0; y < 2; y++) {
				for (int z = 0; z < 2; z++) {
					const auto corner = inverse * glm::vec4{ 2.f * x - 1.f, 2.f * y - 1.f, 2.f * z - 1.f, 1.f };
					frustumCorners[c++] = corner / corner.w;
				}
			}
		}

		// 2. Find view matrix of directional light
		// 2.1. Light will point at center of frustum.
		auto frustumCenter = glm::vec3{ 0.f };
		for (const auto& c : frustumCorners) {
			frustumCenter += glm::vec3(c);
		}
		frustumCenter /= 8.f;

		auto lightView = glm::lookAt(frustumCenter - direction, frustumCenter, { 0.f, 1.f, 0.f });

		// 3. Find projection matrix of directional light
		// 3.1. The camera frustum should fit in the orthographic projection from the light's viewpoint
		float minX = std::numeric_limits<float>::max();
		float maxX = std::numeric_limits<float>::min();
		float minY = std::numeric_limits<float>::max();
		float maxY = std::numeric_limits<float>::min();
		float minZ = std::numeric_limits<float>::max();
		float maxZ = std::numeric_limits<float>::min();

		for (const auto& c : frustumCorners) {
			const auto transform = lightView * c;
			minX = std::min(minX, transform.x);
			maxX = std::max(maxX, transform.x);
			minY = std::min(minY, transform.y);
			maxY = std::max(maxY, transform.y);
			minZ = std::min(minZ, transform.z);
			maxZ = std::max(maxZ, transform.z);
		}

		// 3.2. Extend the near and far planes to include off-screen objects as shadow-casters
		float zScale = 10.f;
		minZ = (minZ < 0) ? minZ * zScale : minZ / zScale;
		maxZ = (maxZ < 0) ? maxZ / zScale : maxZ * zScale;

		auto lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);
		return lightProjection * lightView;
	}

	void ShadowMap::SetupLightAndCamera(const glm::vec3& direction, const Camera& camera, const glm::mat4& camera_transform)
	{
		Camera cascadeCamera = camera;
		auto farPlane = cascadeCamera.GetPerspectiveFar();

		cascadeCamera.SetPerspectiveFar(farPlane / 25.f); // Cascade #1
		//cascadeCamera.SetPerspectiveFar(farPlane / 25.f); // Cascade #2
		//cascadeCamera.SetPerspectiveFar(farPlane / 10.f);	// Cascade #3
		//cascadeCamera.SetPerspectiveFar(farPlane / 2.f);	// Cascade #4

		m_LightSpaceMatrices[0] = CalculateLightViewProjectionMatrix(direction, cascadeCamera, camera_transform);

		m_LightMatricesUBO->SetData(glm::value_ptr(m_LightSpaceMatrices[0]), sizeof(glm::mat4) * m_LightSpaceMatrices.size(), 0);
	}

	void ShadowMap::BuildForScene(const Ref<Scene>& scene)
	{
		m_Framebuffer->Bind();
		RenderCommands::Clear();
		s_ShadowShader->Bind();
		//s_ShadowShader->SetUniMat4("u_LightVP", m_LightSpaceMatrices);
		m_LightMatricesUBO->Bind();
		s_ShadowShader->SetUniFloat2("u_NearFarPlane", near_far_planes);

		auto view = scene->View<MeshRendererComponent>();
		for (auto e : view) {
			Entity entity{ e, scene.get() };
			auto& meshComp = entity.GetComponent<MeshRendererComponent>();
			if (meshComp.castShadows) {
				auto transform = entity.GetComponent<TransformComponent>().GetTransform();
				s_ShadowShader->SetUniMat4("u_Model", transform);
				
				auto& vao = meshComp.mesh->GetVAO();
				if (vao->GetIndexBuffers().empty())
					RenderCommands::Draw(vao);
				else
					RenderCommands::DrawIndexed(vao);
			}
		}
	}

	Ref<Texture> ShadowMap::GetDepthTexture() const
	{
		return m_Framebuffer->GetDepthAttachment();
	}

	Ref<UniformBuffer> ShadowMap::GetLightMatricesUBO() const
	{
		return m_LightMatricesUBO;
	}

	glm::mat4 ShadowMap::GetLightSpaceTransform() const
	{
		return m_LightSpaceMatrices[0];
	}
}
