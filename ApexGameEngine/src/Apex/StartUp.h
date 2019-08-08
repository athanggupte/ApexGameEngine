#pragma once

#ifdef APEX_PLATFORM_WINDOWS

extern Apex::Application* Apex::CreateApplication();

int main(int argc, char** argv)
{
	Apex::Log::Init();

	APEX_CORE_TRACE("TRACE LEVEL");
	APEX_DEBUG("DEBUG LEVEL");
	APEX_CORE_INFO("INFO LEVEL");
	APEX_WARN("WARN LEVEL");
	APEX_CORE_ERROR("ERROR LEVEL");
	APEX_CRITICAL("CRITICAL LEVEL");

	auto app = Apex::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif