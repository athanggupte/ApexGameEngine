#pragma once

#include "Apex/Physics/Collision/Collidable.h"
#include "Apex/Physics/Movement/RigidBody.h"

namespace DXBall {

	class Brick : public Apex::Collidable
	{
	public:
		Brick(glm::vec2 topLeft, glm::vec2 bottomRight)
			: Apex::Collidable(std::make_shared<Apex::AABBCollider>(topLeft, bottomRight))
		{
			m_Position = std::dynamic_pointer_cast<Apex::AABBCollider>(this->GetCollider())->GetCenter();

			APEX_LOG_INFO("Brick position : {0} , {1}", m_Position.x, m_Position.y);

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
				uniform bool u_UseColor;
				
				in vec3 v_Position;
				in vec2 v_TexCoord;

				void main()
				{
					if(u_UseColor) {
						o_Color = u_Color;
					} else {
						vec4 texColor = texture(u_Texture, v_TexCoord).rgba;
						o_Color = vec4(texColor);
					}	
				}
			)";

			m_Shader = Apex::Shader::Create("BrickShader", flatVertexSrc, flatFragmentSrc);
			m_Shader->Bind();
			m_Shader->SetUniInt("u_Texture", 0);
		}

		void Update()
		{
			auto collider = std::dynamic_pointer_cast<Apex::AABBCollider>(m_Collider);
			collider->SetCenter(m_Position);
		}

		void Render()
		{
			auto collider = std::dynamic_pointer_cast<Apex::AABBCollider>(m_Collider);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), { m_Position, 0.0f })
				//* glm::rotate(glm::mat4(1.0f), particle.rotation, { 0.0f, 0.0f, 1.0f })
				* glm::scale(glm::mat4(1.0f), { collider->GetHalfDimensions() * 2.0f, 1.0f });

			/*glm::vec2 texOffset({ particle.textureIndex % particle.textureNumRows, glm::floor(particle.textureIndex / particle.textureNumRows) });
			texOffset = texOffset / (float)particle.textureNumRows;*/

			m_Shader->Bind();
			m_Shader->SetUniFloat4("u_Color", m_Color);
			m_Shader->SetUniFloat1("u_NumRows", 1.0f);
			m_Shader->SetUniFloat2("u_TexOffset", { 0.0f, 0.0f });
			m_Shader->SetUniInt("u_UseColor", m_UseColor);

			Apex::Renderer::Submit(m_Shader, m_QuadVA, transform);
		}

	public:
		bool m_UseColor = true;
		const glm::vec4 c_Color = { 0.13f, 0.35f, 1.0f, 1.0f };
		glm::vec4 m_Color = c_Color;
		glm::vec2 m_Position;

	private:
		Apex::Ref<Apex::VertexArray> m_QuadVA;
		Apex::Ref<Apex::Shader> m_Shader;
	};

}