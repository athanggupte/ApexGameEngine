#pragma once

#include "apex_pch.h"

#include "Apex/Core.h"
#include "Apex/Events/Event.h"

namespace Apex {

	struct WindowProps
	{
		std::string w_Title;
		unsigned int w_Width;
		unsigned int w_Height;

		WindowProps(const std::string& title = "Apex Engine",
					unsigned int width = 1280,
					unsigned int height = 720)
			: w_Title(title), w_Width(width), w_Height(height) {}
	};

	class APEX_API Window
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;

		virtual ~Window() {}

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		// Window Attributes
		virtual void SetEventCallback(const EventCallbackFn& callback) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSync() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};


}