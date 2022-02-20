#pragma once

#include <glm/glm.hpp>

namespace Apex {
	class UniformBuffer;
	class Texture;
	class Shader;
	class Camera;
	class Scene;
	class Framebuffer;

	class ShadowMap // for Directional Light
	{
	public:
		ShadowMap(uint32_t width, uint32_t height, uint32_t cascades = 1);

		void SetupLightAndCamera(const glm::vec3& direction, const Camera& camera, const glm::mat4& camera_transform);
		void BuildForScene(const Ref<Scene>& scene);

		Ref<Texture> GetDepthTexture() const;
		Ref<UniformBuffer> GetLightMatricesUBO() const;
		glm::mat4 GetLightSpaceTransform() const;

	private:
		Ref<Framebuffer> m_Framebuffer;
		Ref<UniformBuffer> m_LightMatricesUBO;
		std::vector<glm::mat4> m_LightSpaceMatrices;
		uint32_t m_Cascades;
	};

}
