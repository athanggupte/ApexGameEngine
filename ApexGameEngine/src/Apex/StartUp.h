#pragma once

#ifdef APEX_PLATFORM_WINDOWS

extern Apex::Application* Apex::CreateApplication();

int main(int argc, char** argv)
{
	Apex::Log::Init();

	APEX_CORE_INFO("Welcome to Apex Game Engine !");

	auto app = Apex::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif