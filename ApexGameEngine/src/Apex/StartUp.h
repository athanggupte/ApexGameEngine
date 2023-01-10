#pragma once
#include <Util/Random.h>
#include "Apex/Utils/Profiler.h"

//#ifdef APEX_PLATFORM_WINDOWS

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
	
	Apex::Random::Init();

	//APEX_PROFILE_BEGIN_SESSION("StartUp", "ApexGameEngineProfile-StartUp.json");
	auto app = Apex::CreateApplication();
	//APEX_PROFILE_END_SESSION();

	Apex::Timer::Init();

	APEX_PROFILE_BEGIN_SESSION("Runtime", "ApexGameEngineProfile-Runtime.json");
	app->Run();
	APEX_PROFILE_END_SESSION();

	//APEX_PROFILE_BEGIN_SESSION("ShutDown", "ApexGameEngineProfile-ShutDown.json");
	delete app;
	//APEX_PROFILE_END_SESSION();

	return 0;
}

//#endif
