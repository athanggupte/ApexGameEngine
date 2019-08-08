#include <Apex.h>

class Sandbox : public Apex::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}

};

Apex::Application* Apex::CreateApplication()
{
	return new Sandbox();
}