#pragma once

#ifdef APEX_PLATFORM_WINDOWS

extern Apex::Application* Apex::CreateApplication();

int main(int argc, char** argv)
{
	Apex::Log::Init();
	/* Start up engine systems -->
	Apex::MemoryManager::Init();
	Apex::FileManager::Init();
	Apex::VideoManager::Init();
	Apex::AudioManager::Init();
	Apex::TextureManager::Init();
	Apex::RenderManager::Init();
	Apex::AnimationManager::Init();
	Apex::PhysicsManager::Init();
	// ...
	*/

	APEX_CORE_INFO("Welcome to Apex Game Engine !");

	auto app = Apex::CreateApplication();
	app->Run();
	delete app;

	return 0;
}

#endif