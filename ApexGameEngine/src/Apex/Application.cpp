#include "apex_pch.h"
#include "Application.h"

#include "Apex/Log/Log.h"
#include "Apex/Input/Input.h"
#include "Apex/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Apex {

#define BIND_CALLBACK_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		APEX_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_CALLBACK_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		/* Triangle Data */
		m_VertexArray.reset(VertexArray::Create());

		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.7f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.7f, 0.8f, 0.5f, 1.0f,
			 0.0f,  0.75f, 0.0f, 0.5f, 0.7f, 0.8f, 1.0f
		};
		std::shared_ptr<VertexBuffer> vertexBuffer;
		vertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
			};
			vertexBuffer->SetLayout(layout);
		}

		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		std::shared_ptr<IndexBuffer> indexBuffer;
		indexBuffer.reset(IndexBuffer::Create(indices, 3));
		m_VertexArray->AddIndexBuffer(indexBuffer);

		m_VertexArray->Unbind();
		/* End Triangle Data */

		std::string vertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = vec4(a_Position, 1.0);
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

		m_Shader.reset(Shader::Create(vertexSrc, fragmentSrc));

		/* Square Data */
		m_SquareVA.reset(VertexArray::Create());

		std::shared_ptr<VertexBuffer> squareVB;
		float squareVertices[] = {
			-0.5f, -0.5f, 0.0f,
			 0.5f, -0.5f, 0.0f,
			 0.5f,  0.5f, 0.0f,
			-0.5f,  0.5f, 0.0f
		};
		squareVB.reset(VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{ ShaderDataType::Float3, "a_Position" }
		});
		m_SquareVA->AddVertexBuffer(squareVB);

		std::shared_ptr<IndexBuffer> squareIB;
		uint32_t squareIndices[] = {
			0, 1, 2,
			0, 2, 3
		};
		squareIB.reset(IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->AddIndexBuffer(squareIB);

		m_SquareVA->Unbind();
		/* End Square Data */

		std::string squareVertexSrc = R"(
			#version 450

			layout(location = 0) in vec3 a_Position;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = vec4(a_Position, 1.0);
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

		m_SquareShader.reset(Shader::Create(squareVertexSrc, squareFragmentSrc));
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		float x = 0;
		while (m_Running) {
			x = x + 0.01f;

			RenderCommands::SetClearColor({ 0.12f, 0.1185f, 0.12f, 1.0f });
			RenderCommands::Clear();
			
			Renderer::BeginScene();

			m_SquareShader->Bind(); glUniform1f(1, x);
			Renderer::Submit(m_SquareVA);

			m_Shader->Bind(); glUniform1f(1, x);
			Renderer::Submit(m_VertexArray);

			Renderer::EndScene();

			for (Layer* layer : m_LayerStack)
				layer->OnUpdate();

			//Render ImGui
			m_ImGuiLayer->Begin();
			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	// Event Handlers

	void Application::OnEvent(Event & e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_CALLBACK_FN(OnWindowClose));

		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin(); ) {
			(*--it)->OnEvent(e);
			if (e.IsHandled())
				break;
		}
	}

	void Application::PushLayer(Layer * layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer * overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent & e)
	{
		m_Running = false;
		return false;
	}

}