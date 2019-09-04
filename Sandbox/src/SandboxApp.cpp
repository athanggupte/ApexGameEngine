#include <Apex.h>
#include <imgui.h>
/*
#include <Apex/Renderer/Shader.h>
#include <Apex/Renderer/VertexArray.h>
#include <Apex/Renderer/Camera.h>
#include <Apex/Renderer/Renderer.h>
*/
class SandboxLayer : public Apex::Layer
{
public:
	SandboxLayer()
		: Layer("Sandbox")//, m_Camera(-1.6f, 1.6f, 0.9f, 0.9f)
	{
		/// Triangle Data ///
	/*	m_VertexArray.reset(Apex::VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.7f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.7f, 0.8f, 0.5f, 1.0f,
			 0.0f,  0.75f, 0.0f, 0.5f, 0.7f, 0.8f, 1.0f
		};
		std::shared_ptr<Apex::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Apex::VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			Apex::BufferLayout layout = {
				{ Apex::ShaderDataType::Float3, "a_Position" },
				{ Apex::ShaderDataType::Float4, "a_Color" }
			};
			vertexBuffer->SetLayout(layout);
		}

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<Apex::IndexBuffer> indexBuffer;
		indexBuffer.reset(Apex::IndexBuffer::Create(indices, 3));
		m_VertexArray->AddIndexBuffer(indexBuffer);

		m_VertexArray->Unbind();
		/// End Triangle Data ///

		std::string vertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string fragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;
			layout(location = 1) uniform float x;
			
			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				o_Color = vec4(v_Color.r * abs(cos(x)), v_Color.g * abs(sin(2 * x)), v_Color.b * abs(sin(3 * x)), v_Color.a);
			}
		)";

		m_Shader.reset(Apex::Shader::Create(vertexSrc, fragmentSrc));

		/// Square Data ///
		m_SquareVA.reset(Apex::VertexArray::Create());

		std::shared_ptr<Apex::VertexBuffer> squareVB;
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		squareVB.reset(Apex::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ Apex::ShaderDataType::Float3, "a_Position" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		std::shared_ptr<Apex::IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB.reset(Apex::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->AddIndexBuffer(squareIB);

		m_SquareVA->Unbind();
		/// End Square Data ///

		std::string squareVertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * vec4(a_Position, 1.0);
			}
		)";

		std::string squareFragmentSrc = R"(
			#version 450

			layout(location = 0) out vec4 o_Color;
			layout(location = 1) uniform float x;
			
			in vec3 v_Position;

			void main()
			{
				o_Color = vec4(abs(cos(x)), abs(sin(2 * x)), abs(sin(3 * x)), 1.0);
			}
		)";

		m_SquareShader.reset(Apex::Shader::Create(squareVertexSrc, squareFragmentSrc));*/
	}

	void OnUpdate() override
	{
	/*	x = x + 0.01f;

		m_Camera.SetPosition({ sin(x), cos(x), 0.f });
		m_Camera.SetRotation(360.f * sin(x));

		Apex::RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
		Apex::RenderCommands::Clear();
		Apex::Renderer::BeginScene(m_Camera);

		m_SquareShader->Bind();
		m_SquareShader->SetUniFloat1("x", x);
		Apex::Renderer::Submit(m_SquareShader, m_SquareVA);

		m_Shader->Bind();
		m_SquareShader->SetUniFloat1("x", x);
		Apex::Renderer::Submit(m_Shader, m_VertexArray);

		Apex::Renderer::EndScene();*/
	}

	void OnEvent(Apex::Event& event) override
	{

	}

	void OnImGuiRender() override
	{

	}
	
private:/*
	std::shared_ptr<Apex::Shader> m_Shader;
	std::shared_ptr<Apex::VertexArray> m_VertexArray;

	std::shared_ptr<Apex::Shader> m_SquareShader;
	std::shared_ptr<Apex::VertexArray> m_SquareVA;
	
	Apex::OrthographicCamera m_Camera;

	float x = 0;
	*/
};


class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{
		PushLayer(new SandboxLayer());
	}

	~Sandbox()
	{

	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}