#pragma once
#include "Apex/Core/ResourceManager/ResourceManager.h"

namespace Apex {

	class Entity;
	class EntityScript;

	class APEX_API AScriptFactory
	{
	public:
		virtual ~AScriptFactory() = default;
		virtual EntityScript* InstantiateScript() = 0;
		virtual void DestroyScript(EntityScript* script) = 0;

	protected:
		// AScriptFactory(Handle id);
	};

}

#define APEX_REGISTER_SCRIPT_FACTORY(script) \
	class SCRIPT_TEST_API script##Factory : public Apex::AScriptFactory \
	{ public: \
		static decltype(auto) Create() \
		{ \
			auto ptr = Apex::Ref<script##Factory>(new script##Factory()); \
			Apex::Application::Get().GetResourceManager().Insert<AScriptFactory>(RESNAME(STR(script)), ptr); \
			return ptr; \
		} \
		Apex::EntityScript* InstantiateScript() override { return new script; } \
		void DestroyScript(Apex::EntityScript* _script) override { delete _script; } \
	}; \
	static Apex::Ref<Apex::AScriptFactory> g_##script = script##Factory::Create();

//class SCRIPT_TEST_API LogScriptFactory : public Apex::AScriptFactory
//{
//public:
//	static decltype(auto) Create()
//	{
//		auto ptr = Apex::Ref<LogScriptFactory>(new LogScriptFactory());
//		Apex::Application::Get().GetResourceManager().Insert<AScriptFactory>(
//			RESNAME(STR(LogScript)), ptr);
//		return ptr;
//	}
//
//	Apex::EntityScript* InstantiateScript() override { return new LogScript; }
//	void DestroyScript(Apex::EntityScript* _script) override { delete _script; }
//
//private:
//	LogScriptFactory() = default;
//};
//
//static Apex::Ref<Apex::AScriptFactory> g_LogScript = LogScriptFactory::Create();