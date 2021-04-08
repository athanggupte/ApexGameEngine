#include "apex_pch.h"
#include "Apex/Core/Input/Input.h"

#include <GLFW/glfw3.h>
#include "Apex/Application.h"

#if defined(APEX_PLATFORM_WINDOWS) || defined(APEX_PLATFORM_LINUX)

namespace Apex {

	bool Input::IsKeyPressed(int keycode)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(window, keycode);
		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(window, button);
		return state == GLFW_PRESS;
	}

	std::pair<float, float> Input::GetMousePos()
	{
		auto window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);

		return { (float)xpos, (float)ypos };
	}

	float Input::GetMouseX()
	{
		auto[x, y] = GetMousePos();
		return x;
	}

	float Input::GetMouseY()
	{
		auto[x, y] = GetMousePos();
		return y;
	}

}

#endif // APEX_PLATFORM_WINDOWS or APEX_PLATFORM_LINUX
