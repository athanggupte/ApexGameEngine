#include "apex_pch.h"
#include "Application.h"

#include "Apex/Log/Log.h"
#include "Apex/Input/Input.h"

#include <glad/glad.h>

namespace Apex {

#define BIND_CALLBACK_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::s_Instance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Apex::ShaderDataType::Int:			return GL_INT;
		case Apex::ShaderDataType::Int2:		return GL_INT;
		case Apex::ShaderDataType::Int3:		return GL_INT;
		case Apex::ShaderDataType::Int4:		return GL_INT;
		case Apex::ShaderDataType::Float:		return GL_FLOAT;
		case Apex::ShaderDataType::Float2:		return GL_FLOAT;
		case Apex::ShaderDataType::Float3:		return GL_FLOAT;
		case Apex::ShaderDataType::Float4:		return GL_FLOAT;
		case Apex::ShaderDataType::Mat2:		return GL_FLOAT;
		case Apex::ShaderDataType::Mat3:		return GL_FLOAT;
		case Apex::ShaderDataType::Mat4:		return GL_FLOAT;
		case Apex::ShaderDataType::Bool:		return GL_BOOL;
		default:	APEX_CORE_CRITICAL("Unknown ShaderDataType!");	return 0;
		}
	}

	Application::Application()
	{
		APEX_CORE_ASSERT(!s_Instance, "Application already exists.");
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_CALLBACK_FN(OnEvent));

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		/* Triangle Data */
		glGenVertexArrays(1, &m_VertexArray);
		glBindVertexArray(m_VertexArray);


		float vertices[] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.5f, 0.7f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.7f, 0.8f, 0.5f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.5f, 0.7f, 0.8f, 1.0f
		};
		m_VertexBuffer.reset(VertexBuffer::Create(vertices, sizeof(vertices)));

		{
			BufferLayout layout = {
				{ ShaderDataType::Float3, "a_Position" },
				{ ShaderDataType::Float4, "a_Color" }
			};
			m_VertexBuffer->SetLayout(layout);
		}

		uint32_t index = 0;
		const BufferLayout& layout = m_VertexBuffer->GetLayout();
		for (const auto& element : layout) {
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index,
				element.GetComponentCount(),
				ShaderDataTypeToOpenGLBaseType(element.e_Type),
				element.e_Normalized ? GL_TRUE : GL_FALSE,
				layout.GetStride(),
				(const void*)element.e_Offset);
			index++;
		}
		

		unsigned int indices[3] = { 0, 1, 2 };
		m_IndexBuffer.reset(IndexBuffer::Create(indices, 3));

		glBindVertexArray(0);
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
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		float x = 0;
		while (m_Running) {
			x = x + 0.01f;
			glClearColor(0.12f, 0.1185f, 0.12f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			m_Shader->Bind();
			glUniform1f(1, x);
			glBindVertexArray(m_VertexArray);
			glDrawElements(GL_TRIANGLES, m_IndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);
			glBindVertexArray(0);

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