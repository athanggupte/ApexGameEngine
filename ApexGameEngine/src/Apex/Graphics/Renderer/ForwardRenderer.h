#pragma once

#include <glm/glm.hpp>

namespace Apex {
	
	class RenderCamera;
	class Camera;
	class Shader;
	class Mesh;
	
	class ForwardRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const RenderCamera& camera, const glm::mat4& transform);
		static void BeginScene(const Camera& camera);
		static void EndScene();
		
		static void SetupLights();
		
		static void SubmitMesh(const Ref<Mesh>& mesh, const Ref<Shader>& shader, const glm::mat4& modelTransform);
		
		static uint32_t GetNumDrawCalls();
		
	};
	
}
