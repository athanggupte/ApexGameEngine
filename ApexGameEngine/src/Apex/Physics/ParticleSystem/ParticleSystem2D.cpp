#include "apex_pch.h"
#include "ParticleSystem2D.h"

#include "Apex/Core/Timer.h"
#include "Apex/Core/Random.h"
#include "Apex/Graphics/Renderer/Renderer.h"

#include "Apex/Utils/MathPrimitiveParser.h"

#include <glm/gtc/constants.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/compatibility.hpp>

namespace Apex {
	
	ParticleSystem2D::ParticleSystem2D(uint32_t batch_size, uint32_t max_particles)
		: m_BatchSize(batch_size), m_PoolIndex(max_particles - 1)
	{
		m_ParticlePool.resize(max_particles);
		APEX_CORE_INFO("Particle Pool Size : {0}", m_ParticlePool.size());

		m_QuadVA = Apex::VertexArray::Create();

		Apex::Ref<Apex::VertexBuffer> squareVB;
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		squareVB = Apex::VertexBuffer::Create(squareVertices, sizeof(squareVertices));
		squareVB->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" },
			{ Apex::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_QuadVA->AddVertexBuffer(squareVB);

		Apex::Ref<Apex::IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB = Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
		m_QuadVA->AddIndexBuffer(squareIB);

		m_QuadVA->Unbind();

		std::string flatVertexSrc = R"(
			#version 430

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec2 a_TexCoord;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Model;
			uniform float u_NumRows;
			uniform vec2 u_TexOffset;

			out vec3 v_Position;
			out vec2 v_TexCoord;

			void main()
			{
				vec2 texCoord = a_TexCoord;
				if(u_NumRows > 1) {
					texCoord = (texCoord / u_NumRows) + u_TexOffset;
				}
				v_Position = a_Position;
				v_TexCoord = vec2(texCoord.x, texCoord.y * -1.0);
				gl_Position = u_ViewProjection * u_Model * vec4(a_Position, 1.0);
			}
		)";

		std::string flatFragmentSrc = R"(
			#version 430

			layout(location = 0) out vec4 o_Color;

			uniform vec4 u_Color;
			uniform sampler2D u_Texture;
			uniform bool u_UseTexture;
			
			in vec3 v_Position;
			in vec2 v_TexCoord;

			void main()
			{
				if(u_UseTexture) {
					vec4 texColor = texture(u_Texture, v_TexCoord).rgba;
					o_Color = vec4(texColor * u_Color);
					//o_Color = vec4(1.0, 0.0, 0.0, 1.0);
				} else {
					o_Color = u_Color;
				}
			}
		)";

		m_Shader = Apex::Shader::Create("TextureShader", flatVertexSrc, flatFragmentSrc);
		m_Shader->Bind();
		m_Shader->SetUniInt("u_Texture", 0);
	}

	void ParticleSystem2D::OnUpdate()
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

			particle.lifeRemaining -= Timer::GetSeconds();
			particle.position += particle.velocity * Timer::GetSeconds();
			particle.velocity += particle.acceleration * Timer::GetSeconds();
			particle.rotation += particle.rotationSpeed * Timer::GetSeconds();
		}
	}

	void ParticleSystem2D::OnRender()
	{
		uint32_t i = 0;
		static bool first_active = true;
		for (auto& particle : m_ParticlePool) {
			i++;
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

// 			if (first_active) {
// 				APEX_CORE_DEBUG("sizeEnd: {0}, {1}", particle.sizeEnd.x, particle.sizeEnd.y);
// 				APEX_CORE_DEBUG("sizeBegin: {0}, {1}", particle.sizeBegin.x, particle.sizeBegin.y);
// 				APEX_CORE_DEBUG("life: {0}", life);
// 				APEX_CORE_DEBUG("size: {0}, {1}", size.x, size.y);
// 				APEX_CORE_DEBUG("scale: \n{0}", MathParser::ParseMatrix(glm::scale(glm::mat4(1.0f), { size, 1.0 })));
// 				APEX_CORE_DEBUG(MathParser::ParseMatrix(transform));
// 				first_active = false;
// 			}
			
			m_Shader->Bind();
			
			if (particle.useTexture) {
				glm::vec2 texOffset({ particle.textureIndex % particle.textureNumRows, glm::floor(particle.textureIndex / particle.textureNumRows) });
				texOffset = texOffset / (float)particle.textureNumRows;
				m_Shader->SetUniFloat2("u_TexOffset", texOffset);
			}

			m_Shader->SetUniFloat4("u_Color", color);
			m_Shader->SetUniFloat1("u_NumRows", particle.textureNumRows);
			m_Shader->SetUniInt("u_UseTexture", particle.useTexture);

			Renderer::Submit(m_Shader, m_QuadVA, transform);
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
