#pragma once

#include "Apex/Window.h"
#include "Apex/Graphics/Renderer/RenderingContext.h"

#include <GLFW/glfw3.h>

namespace Apex {

	class GLFWWindow : public Window
	{
	public:
		GLFWWindow(const WindowProps& props);
		virtual ~GLFWWindow();

		void OnUpdate() override;

		inline uint32_t GetWidth() const override { return m_Data.w_Width; }
		inline uint32_t GetHeight() const override { return m_Data.w_Height; }

		inline void SetEventCallback(const EventCallbackFn& callback) override { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled) override;
		bool IsVSync() const override;

		void SetWindowIcon(const ImageData& imageData) const override;
		
		inline virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		GLFWwindow* m_Window;
		RenderingContext* m_Context;

		struct WindowData
		{
			std::string w_Title;
			uint32_t w_Width, w_Height;
			bool VSync;

			EventCallbackFn EventCallback;
		};

		WindowData m_Data;
	};

}
