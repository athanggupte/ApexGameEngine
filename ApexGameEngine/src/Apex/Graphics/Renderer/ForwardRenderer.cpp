#include "apex_pch.h"
#include "ForwardRenderer.h"

#include "RenderCommands.h"

#include "Apex/Graphics/RenderPrimitives/Shader.h"
#include "Apex/Graphics/RenderPrimitives/VertexArray.h"
#include "Apex/Graphics/RenderPrimitives/Buffer.h"

#include "Apex/Graphics/Mesh.h"

#include "Apex/Core/Camera.h"

#include "Apex/Utils/Utils.h"
#include "Apex/Utils/Profiler.h"
#include "Apex/Utils/MathPrimitiveParser.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Apex {
	
	struct RenderCommand
	{
		Ref<Mesh> mesh;
		Ref<Shader> shader;
		std::vector<glm::mat4> transforms;
	};
	
	/**
	 * Render Uniform Buffer in GLSL:
	 * layout(std140, binding = 1) uniform RenderUniforms
	 * {
	 *     mat4 Projection;
	 *     mat4 View;
	 *     vec4 DL_Direction[MAX_DIR_LIGHTS];
	 *     mat4 Model;
	 * };
	 * 
	 */
	enum RenderUniformId : uint32_t
	{
		Camera_Projection,
		Camera_View,
		Camera_Position,
		
		DirLights_Direction,
		DirLights_Color,
		DirLights_Intensity,
		
		PointLights_Position,
		PointLights_Color,
		PointLights_Intensity,
		PointLights_Attenuation,
		
		Mesh_Transform,
		Mesh_NormalMat,
		
		NUM_UNIFORMS
	};
	
	struct RendererData
	{
		Ref<UniformBuffer> RenderUniformBuffer;
		Ref<ShaderStorageBuffer> RenderStorageBuffer;
		
		void* MappedUniformBufferPtr = nullptr;
		
		std::vector<RenderCommand> Commands;
		
#if 0		
		// I. Camera Data
		uint32_t ProjectionOffset;
		uint32_t ViewOffset;
		uint32_t EyePositionOffset;
		
		const uint32_t ProjectionSize = sizeof(glm::mat4);
		const uint32_t ViewSize = sizeof(glm::mat4);
		const uint32_t EyePositionSize = sizeof(glm::vec4);
		
		// II. Lights
		// II.1. Directional Lights
		static const uint32_t MAX_DIR_LIGHTS = 1;
		
		uint32_t DirectionOffsets[MAX_DIR_LIGHTS];
		uint32_t 
		
		const uint32_t DirectionSize = sizeof(glm::vec4);
		
		// II.2. Point Lights
		
		// II.3. Spot Lights
		
		// III. Per Mesh Data
		// III.1. Transform
		uint32_t ModelMatrixOffset;
		uint32_t NormalMatrixOffset;
		
		const uint32_t ModelMatrixSize = sizeof(glm::mat4);
		const uint32_t NormalMatrixSize = sizeof(glm::mat4);
#endif
		std::array<std140::DataAlignment, NUM_UNIFORMS> UniformAlignments;
		std::array<uint32_t, NUM_UNIFORMS> UniformOffsets;
		std::array<uint32_t, NUM_UNIFORMS> UniformSizes;
		
		static const uint32_t MAX_DIR_LIGHTS = 1;
		static const uint32_t MAX_POINT_LIGHTS = 2;
		
		uint32_t SizeInBuffer = 0;
		
		std::unordered_map<std::pair<Ref<Mesh>, Ref<Shader>>, uint32_t> MeshShaderIndexCache;
		
		// Convenience Functions
		inline void SetGlobalUniform(const void* data, RenderUniformId index)
		{
			APEX_PROFILE_FUNC();
			// this->RenderUniformBuffer->SetData(data, this->UniformSizes[index], this->UniformOffsets[index]);
			APEX_CORE_ASSERT(this->MappedUniformBufferPtr != nullptr, "[ForwardRenderer] :: Begin Scene before setting data!");
			memcpy((char*)this->MappedUniformBufferPtr + this->UniformOffsets[index], data, this->UniformSizes[index]);
		}
		
		// Stats
		uint32_t NumDrawCalls = 0;
	};
	
	static RendererData* s_Data;
	
	void ForwardRenderer::Init()
	{
		APEX_PROFILE_FUNC();
		
		s_Data = new RendererData();
		
		uint32_t accumulator = 0;
		
#if 0
		// -- Set uniform offsets --
		// Camera uniforms
		s_Data->ProjectionOffset = accumulator; accumulator += s_Data->ProjectionSize;
		s_Data->ViewOffset = accumulator; accumulator += s_Data->ViewSize;
		s_Data->EyePositionOffset = accumulator; accumulator += s_Data->EyePositionSize;
		// Direction Lights
		for (auto i=0; i < s_Data->MAX_DIR_LIGHTS; i++) {
			s_Data->DirectionOffsets[i] = accumulator; accumulator += s_Data->DirectionSize;
		}
		// Point Lights
		// Spot Lights
		// Mesh Data
		s_Data->ModelMatrixOffset = accumulator; accumulator += s_Data->ModelMatrixSize;
		s_Data->NormalMatrixOffset = accumulator; accumulator += s_Data->NormalMatrixSize;
		
#endif
		// -- Alignments --
		s_Data->UniformAlignments[Camera_Projection]    = std140::Mat4;
		s_Data->UniformAlignments[Camera_View]          = std140::Mat4;
		s_Data->UniformAlignments[Camera_Position]      = std140::Vec4;
		
		s_Data->UniformAlignments[DirLights_Direction]    = std140::Vec4;
		s_Data->UniformAlignments[DirLights_Color]        = std140::Vec4;
		s_Data->UniformAlignments[DirLights_Intensity]    = std140::Vec4;
		
		s_Data->UniformAlignments[PointLights_Position]       = std140::Vec4;
		s_Data->UniformAlignments[PointLights_Color]          = std140::Vec4;
		s_Data->UniformAlignments[PointLights_Intensity]      = std140::Vec4;
		s_Data->UniformAlignments[PointLights_Attenuation]    = std140::Vec4;
		
		s_Data->UniformAlignments[Mesh_Transform]    = std140::Mat4;
		s_Data->UniformAlignments[Mesh_NormalMat]    = std140::Mat4;
		
		// -- Sizes --
		s_Data->UniformSizes[Camera_Projection]    = sizeof(glm::mat4);
		s_Data->UniformSizes[Camera_View]          = sizeof(glm::mat4);
		s_Data->UniformSizes[Camera_Position]      = sizeof(glm::vec4);
		
		s_Data->UniformSizes[DirLights_Direction]    = sizeof(glm::vec4) * s_Data->MAX_DIR_LIGHTS;
		s_Data->UniformSizes[DirLights_Color]        = sizeof(glm::vec4) * s_Data->MAX_DIR_LIGHTS;
		s_Data->UniformSizes[DirLights_Intensity]    = sizeof(glm::vec4) * s_Data->MAX_DIR_LIGHTS;
		
		s_Data->UniformSizes[PointLights_Position]       = sizeof(glm::vec4) * s_Data->MAX_POINT_LIGHTS;
		s_Data->UniformSizes[PointLights_Color]          = sizeof(glm::vec4) * s_Data->MAX_POINT_LIGHTS;
		s_Data->UniformSizes[PointLights_Intensity]      = sizeof(glm::vec4) * s_Data->MAX_POINT_LIGHTS;
		s_Data->UniformSizes[PointLights_Attenuation]    = sizeof(glm::vec4) * s_Data->MAX_POINT_LIGHTS;
		
		s_Data->UniformSizes[Mesh_Transform]    = sizeof(glm::mat4);
		s_Data->UniformSizes[Mesh_NormalMat]    = sizeof(glm::mat4);
		
		// -- Offsets --
		for (uint32_t i=0; i<NUM_UNIFORMS; i++) {
			s_Data->UniformOffsets[i] = accumulator;
			if (i<NUM_UNIFORMS-1)
				accumulator = std140::GetNextPosition(accumulator, s_Data->UniformSizes[i], s_Data->UniformAlignments[i+1]);
			else
				accumulator += s_Data->UniformSizes[i];
			
			APEX_CORE_TRACE("Uniform [{0}] :: Size: {1}, Offset: {2}", i, s_Data->UniformSizes[i], s_Data->UniformOffsets[i]);
		}
		
		APEX_CORE_DEBUG("Size of Uniform Buffer: {}", accumulator);
		
		// -- Create Uniform buffer --
		s_Data->SizeInBuffer = accumulator;
		s_Data->RenderUniformBuffer = UniformBuffer::Create(s_Data->SizeInBuffer, /* binding */ 1);
		
		s_Data->RenderStorageBuffer = ShaderStorageBuffer::Create(0, 2);
	}
	
	void ForwardRenderer::Shutdown()
	{
		delete s_Data;
	}
	
	void ForwardRenderer::BeginScene(const RenderCamera& camera, const glm::mat4& transform)
	{
		APEX_PROFILE_FUNC();
		
		APEX_CORE_DEBUG("MeshShaderIndexCache :: size : {}", s_Data->MeshShaderIndexCache.size());
		// s_Data->MeshShaderIndexCache.clear();
		
		s_Data->MappedUniformBufferPtr = s_Data->RenderUniformBuffer->MapBuffer(false);
		
		s_Data->SetGlobalUniform(glm::value_ptr(camera.GetProjection()), Camera_Projection);
		s_Data->SetGlobalUniform(glm::value_ptr(glm::inverse(transform)), Camera_View);
		s_Data->SetGlobalUniform(glm::value_ptr(glm::vec4(transform[3])), Camera_Position);
	}
	
	void ForwardRenderer::BeginScene(const Camera& camera)
	{
		APEX_PROFILE_FUNC();
		
		// s_Data->MeshShaderIndexCache.clear();
		
		s_Data->MappedUniformBufferPtr = s_Data->RenderUniformBuffer->MapBuffer(false);
		
		s_Data->SetGlobalUniform(glm::value_ptr(camera.GetProjectionMatrix()), Camera_Projection);
		s_Data->SetGlobalUniform(glm::value_ptr(camera.GetViewMatrix()), Camera_View);
		s_Data->SetGlobalUniform(glm::value_ptr(glm::vec4(glm::inverse(camera.GetViewMatrix())[3])), Camera_Position);
	}
	
	void ForwardRenderer::EndScene()
	{
		APEX_PROFILE_FUNC();
		
		s_Data->RenderUniformBuffer->UnmapBuffer();
		s_Data->MappedUniformBufferPtr = nullptr;
		
		s_Data->NumDrawCalls = 0;
		for (auto& command : s_Data->Commands) {
			// auto& command = s_Data->Commands.front();
			auto& mesh = command.mesh;
			if (!mesh->GetVAO())
				continue;
			
			command.shader->Bind();
			
			uint32_t offsetSize = sizeof(glm::mat4) * 2;
			uint32_t sizeReqd = offsetSize * command.transforms.size();
			
			if (s_Data->RenderStorageBuffer->GetSize() < sizeReqd)
				s_Data->RenderStorageBuffer->ResetData(sizeReqd);
			
			void * mappedStorageBufferPtr = s_Data->RenderStorageBuffer->MapBuffer(false);
			uint32_t offset = 0;
			for (auto& transform : command.transforms) {
				// s_Data->MappedUniformBufferPtr = s_Data->RenderUniformBuffer->MapBuffer(false);
				// s_Data->RenderStorageBuffer->ResetData(sizeof(glm::mat4) * 2 * command.transforms.size());
				
				// s_Data->SetGlobalUniform(glm::value_ptr(transform), Mesh_Transform);
				
				glm::mat4 normalMatrix = glm::mat4(glm::mat3(glm::transpose(glm::inverse(transform))));
				// s_Data->SetGlobalUniform(glm::value_ptr(normalMatrix), Mesh_NormalMat);
				
				memcpy((char*)mappedStorageBufferPtr + offset, glm::value_ptr(transform), sizeof(transform));
				offset += sizeof(transform);
				
				memcpy((char*)mappedStorageBufferPtr + offset, glm::value_ptr(normalMatrix), sizeof(normalMatrix));
				offset += sizeof(normalMatrix);
				
				// RenderCommands::DrawIndexed(mesh->GetVAO());
				
				// s_Data->NumDrawCalls++;
				
				// s_Data->RenderUniformBuffer->UnmapBuffer();
				// s_Data->MappedUniformBufferPtr = nullptr;
				
				//APEX_CORE_DEBUG("offset : {}", offset);
			}
			s_Data->NumDrawCalls++;
			s_Data->RenderStorageBuffer->UnmapBuffer();
			RenderCommands::DrawInstanced(mesh->GetVAO(), command.transforms.size());
			
			command.transforms.clear();
		}
		
		// memset(&s_Data->Commands[0], 0, s_Data->Commands.size() * sizeof(s_Data->Commands[0]));
		
		// s_Data->RenderUniformBuffer->UnmapBuffer();
		// s_Data->MappedUniformBufferPtr = nullptr;
	
	}
	
	void ForwardRenderer::SetupLights()
	{
		glm::vec4 DL_Dirs[s_Data->MAX_DIR_LIGHTS];
		glm::vec4 DL_Colors[s_Data->MAX_DIR_LIGHTS];
		glm::vec4 DL_Intensities[s_Data->MAX_DIR_LIGHTS];
		
		for (auto i=0; i < s_Data->MAX_DIR_LIGHTS; i++) {
			DL_Dirs[i] = glm::normalize(glm::vec4{ 1.f, 1.f, -5.f, 0.f });
			DL_Colors[i] = { 0.93f, 0.82f, 0.75f, 1.f };
			DL_Intensities[i] = { 0.2f, 1.f, 0.5f, 0.f };
		}
		
		s_Data->SetGlobalUniform(DL_Dirs, DirLights_Direction);
		s_Data->SetGlobalUniform(DL_Colors, DirLights_Color);
		s_Data->SetGlobalUniform(DL_Intensities, DirLights_Intensity);
		
		
		glm::vec4 PL_Pos[s_Data->MAX_POINT_LIGHTS];
		glm::vec4 PL_Colors[s_Data->MAX_POINT_LIGHTS];
		glm::vec4 PL_Intensities[s_Data->MAX_POINT_LIGHTS];
		glm::vec4 PL_Attenuation[s_Data->MAX_POINT_LIGHTS];
		
		for (auto i=0; i < s_Data->MAX_POINT_LIGHTS; i++) {
			float modifier = (float)(2 * i - 1);
			PL_Pos[i] = { 5.f * modifier, 2.5f * modifier, 1.f, 0.f };
			PL_Colors[i] = { 1.f * (float)i, 1.f * (float)(1-i), 0.63f, 1.f };
			PL_Intensities[i] = { 0.2f, 1.f, 0.5f, 0.f };
			PL_Attenuation[i] = { 1.f, 0.09f, 0.032f, 50.f };
		}
		
		s_Data->SetGlobalUniform(PL_Pos, PointLights_Position);
		s_Data->SetGlobalUniform(PL_Colors, PointLights_Color);
		s_Data->SetGlobalUniform(PL_Intensities, PointLights_Intensity);
		s_Data->SetGlobalUniform(PL_Attenuation, PointLights_Attenuation);
	}
	
	void ForwardRenderer::SubmitMesh(const Ref<Mesh>& mesh, const Ref<Shader>& shader, const glm::mat4& modelTransform)
	{
		APEX_PROFILE_FUNC();
		
		auto cacheIt = s_Data->MeshShaderIndexCache.find({ mesh, shader });
		if (cacheIt != s_Data->MeshShaderIndexCache.end()) {
			s_Data->Commands[cacheIt->second].transforms.push_back(modelTransform);
		}
		else {
			s_Data->Commands.push_back({ mesh, shader, { modelTransform } });
			s_Data->MeshShaderIndexCache.insert({ { mesh, shader }, (uint32_t)(s_Data->Commands.size() - 1) });
		}
// 		s_Data->SetGlobalUniform(glm::value_ptr(modelTransform), Mesh_Transform);
// 		
// 		glm::mat4 normalMatrix = glm::mat4(glm::mat3(glm::transpose(glm::inverse(modelTransform))));
// 		s_Data->SetGlobalUniform(glm::value_ptr(normalMatrix), Mesh_NormalMat);
// 		
// 		shader->Bind();
// 		RenderCommands::DrawIndexed(mesh->GetVAO());
	}
	
	uint32_t ForwardRenderer::GetNumDrawCalls()
	{
		return s_Data->NumDrawCalls;
	}
	
}
