#pragma once

#ifdef APEX_PLATFORM_WINDOWS

extern Apex::Application* Apex::CreateApplication();

int main(int argc, char** argv)
{
	printf("Starting Apex Engine ...\n");

	auto app = Apex::CreateApplication();
	app->run();
	delete app;

	return 0;
}

#endif