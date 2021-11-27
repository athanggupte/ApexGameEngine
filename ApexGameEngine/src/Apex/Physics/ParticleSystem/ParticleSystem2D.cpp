#include "apex_pch.h"
#include "ParticleSystem2D.h"

#include "Apex/Utils/Random.h"
#include "Apex/Graphics/Renderer/Renderer.h"

#include "Apex/Utils/MathPrimitiveParser.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Apex {
	
	static std::vector<glm::mat4> s_Transforms;
	static std::vector<glm::vec4> s_Colors;
	static std::vector<int> s_Alive;
	static uint32_t s_BatchIdx;
	
	static void ResetBatch()
	{
		s_BatchIdx = 0;
		memset(&s_Alive[0], 0, sizeof(s_Alive[0]) * s_Alive.size());
		// More data to reset if required
	}
	
	ParticleSystem2D::ParticleSystem2D(uint32_t batch_size, uint32_t max_particles)
		: m_BatchSize(batch_size), m_PoolIndex(max_particles - 1)
	{
		m_ParticlePool.resize(max_particles);
		s_Transforms.resize(m_BatchSize);
		s_Colors.resize(m_BatchSize);
		s_Alive.resize(m_BatchSize);
		
		APEX_CORE_INFO("Particle Pool Size : {0}", m_ParticlePool.size());
		APEX_CORE_INFO("Particle Batch Size : {0}", m_BatchSize);

		m_QuadVA = VertexArray::Create();

		Ref<VertexBuffer> squareVB;
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, VertexElementType::Position },
			{ ShaderDataType::Float2, VertexElementType::UV0 }
			});
		m_QuadVA->AddVertexBuffer(squareVB);

		Ref<IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_QuadVA->AddIndexBuffer(squareIB);

		m_QuadVA->Unbind();

		std::string flatVertexSrc = R"(
			#version 430

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model[)" + std::to_string(m_BatchSize) + R"(];
			uniform float u_NumRows;
			uniform vec2 u_TexOffset;

			out vec3 v_Position;
			out vec2 v_TexCoord;
			flat out int v_InstanceID;

			void main()
			{
				vec2 texCoord = a_TexCoord;
				if(u_NumRows > 1) {
					texCoord = (texCoord / u_NumRows) + u_TexOffset;
				}
				v_Position = a_Position;
				v_TexCoord = vec2(texCoord.x, texCoord.y * -1.0);
				gl_Position = u_ViewProjection * u_Model[gl_InstanceID] * vec4(a_Position, 1.0);
				v_InstanceID = gl_InstanceID;
			}
		)";

		std::string flatFragmentSrc = R"(
			#version 430

			layout(location = 0) out vec4 o_Color;

			uniform vec4 u_Color[)" + std::to_string(m_BatchSize) + R"(];
			uniform bool u_Alive[)"+ std::to_string(m_BatchSize) + R"(];
			uniform sampler2D u_Texture;
			uniform bool u_UseTexture;
			
			in vec3 v_Position;
			in vec2 v_TexCoord;
			flat in int v_InstanceID;

			void main()
			{
				if(u_UseTexture) {
					vec4 texColor = texture(u_Texture, v_TexCoord).rgba;
					o_Color = vec4(texColor * u_Color[v_InstanceID]);
					//o_Color = vec4(1.0, 0.0, 0.0, 1.0);
				} else {
					o_Color = u_Color[v_InstanceID];
				}
			}
		)";

		m_Shader = Shader::Create("TextureShader", flatVertexSrc, flatFragmentSrc);
		m_Shader->Bind();
		m_Shader->SetUniInt1("u_Texture", 0);
	}

	void ParticleSystem2D::OnUpdate(Timestep dt)
	{
        uint32_t i = 0;
		for(auto& particle : m_ParticlePool) {
			i++;
            if (!particle.active)
				continue;

			if (particle.lifeRemaining <= 0.0f) {
                //APEX_CORE_DEBUG("Killed Particle @ {0}", i);
				particle.active = false;
				continue;
			}

			particle.lifeRemaining -= (float)dt;
			particle.position += particle.velocity * (float)dt;
			particle.velocity += particle.acceleration * (float)dt;
			particle.rotation += particle.rotationSpeed * (float)dt;
		}
	}

	void ParticleSystem2D::OnRender()
	{		
		ResetBatch();
		static bool first_active = true;
		for (auto& particle : m_ParticlePool) {
			if (!particle.active)
				continue;

			//APEX_CORE_DEBUG("Rendering Particle @ {0}", i);
			
// 			if (first_active) {
// 				APEX_CORE_DEBUG("Submitted Particle @ {0}", i);
// 				first_active = false;
// 			}

			float life = particle.lifeRemaining / particle.lifetime;
			glm::vec4 color = glm::lerp(particle.colorEnd, particle.colorBegin, life);
			color.a = color.a * life;

			glm::vec2 size = glm::lerp(particle.sizeEnd, particle.sizeBegin, life);

			// Render
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { particle.position, 0.0f })
				* glm::rotate(glm::mat4(1.0f), particle.rotation, { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { size, 1.0 });
			
			if (s_BatchIdx >= m_BatchSize) {
				
				APEX_CORE_DEBUG("Rendering Batch");
				
				m_Shader->Bind();
				
				/*if (particle.useTexture) {
					glm::vec2 texOffset({ particle.textureIndex % particle.textureNumRows, glm::floor(particle.textureIndex / particle.textureNumRows) });
					texOffset = texOffset / (float)particle.textureNumRows;
					m_Shader->SetUniFloat2("u_TexOffset", texOffset);
				}*/

				m_Shader->SetUniFloat4v("u_Color", s_Colors.data(), s_Colors.size());
				m_Shader->SetUniInt1v("u_Alive", s_Alive.data(), s_Alive.size());
				m_Shader->SetUniFloat1("u_NumRows", particle.textureNumRows);
				m_Shader->SetUniInt1("u_UseTexture", false/*particle.useTexture*/);

				Renderer::Submit(m_Shader, m_QuadVA, transform);
				Renderer::SubmitArray(m_Shader, m_QuadVA, s_Transforms.data(), s_Transforms.size());
				
				ResetBatch();
			}
			s_Transforms[s_BatchIdx] = transform;
			s_Colors[s_BatchIdx] = color;
			s_Alive[s_BatchIdx] = 1u;
			s_BatchIdx++;
			
			//APEX_CORE_DEBUG("Submitted Particle @ {0}", i);
		}
	}

	void ParticleSystem2D::Emit(const ParticleProps2D & particleProps)
	{
		
		Particle& particle = m_ParticlePool[m_PoolIndex];
		particle.active = true;
		particle.position = particleProps.position;
		//particle.rotation = Random::Float() * 2.0f * glm::pi<float>();

		particle.acceleration = particleProps.acceleration;

		// Velocity
		particle.velocity = particleProps.velocity;
		particle.velocity.x += particleProps.velocityVariation.x * (Random::Float() - 0.5f);
		particle.velocity.y += particleProps.velocityVariation.y * (Random::Float() - 0.5f);

		// Rotation Speed
		particle.rotation = particleProps.rotation;
		particle.rotationSpeed = particleProps.rotationSpeed;
		particle.rotationSpeed += particleProps.rotationSpeedVariation * (Random::Float() - 0.5f);

		// Color
		particle.colorBegin = particleProps.colorBegin;
		particle.colorEnd = particleProps.colorEnd;

		// Lifetime
		particle.lifetime = particleProps.lifetime;
		particle.lifeRemaining = particleProps.lifetime;

		// Size
		particle.sizeBegin = particleProps.sizeBegin + particleProps.sizeVariation * glm::vec2(Random::Float() - 0.5f) * 0.5f;
		particle.sizeEnd = particleProps.sizeEnd - particleProps.sizeVariation * glm::vec2(Random::Float() - 0.5f) * 0.5f;

		// Texture
		particle.useTexture = particleProps.useTexture;
		particle.textureNumRows = particleProps.textureNumRows;
		particle.textureIndex = particleProps.textureIndex;

		
// 		APEX_CORE_DEBUG("Emit :: Props : sizeEnd: {0}, {1}", particleProps.sizeEnd.x, particleProps.sizeEnd.y);
// 		APEX_CORE_DEBUG("Emit :: Props : sizeBegin: {0}, {1}", particleProps.sizeBegin.x, particleProps.sizeBegin.y);
// 		APEX_CORE_DEBUG("Emit :: Props : sizeVariation: {0}, {1}", particleProps.sizeVariation.x, particleProps.sizeVariation.y);
// 		
// 		APEX_CORE_DEBUG("Emitted Particle @ {0}", m_PoolIndex + 1);
// 		APEX_CORE_DEBUG("Emit :: Particle : sizeEnd: {0}, {1}", particle.sizeEnd.x, particle.sizeEnd.y);
// 		APEX_CORE_DEBUG("Emit :: Particle : sizeBegin: {0}, {1}", particle.sizeBegin.x, particle.sizeBegin.y);
		
		m_PoolIndex = m_ParticlePool.size() - ((m_ParticlePool.size() - m_PoolIndex--) % m_ParticlePool.size()) - 1;
	}

}
