#pragma once
#include "Apex/Core/ResourceManager/ResourceManager.h"
#include "Reflection.h"

namespace Apex {

	class Entity;
	class EntityScript;

	class APEX_API AScriptFactory
	{
	public:
		virtual ~AScriptFactory() = default;
		virtual EntityScript* InstantiateScript() = 0;
		virtual EntityScript* CloneScript(EntityScript* from) = 0;
		virtual void DestroyScript(EntityScript* script) = 0;
		virtual Type* GetType() const = 0;

	protected:
		// AScriptFactory(Handle id);
	};

}

#define APEX_REGISTER_SCRIPT_FACTORY(script) \
	class SCRIPT_TEST_API script##Factory : public Apex::AScriptFactory \
	{ public: \
		static decltype(auto) Create() \
		{ \
			auto ptr = Apex::CreateRef<script##Factory>(); \
			Apex::Application::Get().GetResourceManager().Insert<AScriptFactory>(RESNAME(STR(script)), ptr); \
			APEX_CORE_DEBUG("Added script factory (" STR(script) ")"); \
			return ptr; \
		} \
		Apex::EntityScript* InstantiateScript() override { return new script; } \
		Apex::EntityScript* CloneScript(Apex::EntityScript* from) override { return dynamic_cast<script*>(from) ? new script(*dynamic_cast<script*>(from)) : new script; } \
		void DestroyScript(Apex::EntityScript* _script) override { delete static_cast<script*>(_script); } \
		Type* GetType() const override { return nullptr; } \
	}; \
	static Apex::Ref<Apex::AScriptFactory> g_##script = script##Factory::Create();

#define APEX_REGISTER_SCRIPT_FACTORY_WITH_REFLECTION(script) \
	class SCRIPT_TEST_API script##Factory : public Apex::AScriptFactory \
	{ public: \
		static decltype(auto) Create() \
		{ \
			auto ptr = Apex::CreateRef<script##Factory>(); \
			Apex::Application::Get().GetResourceManager().Insert<AScriptFactory>(RESNAME(STR(script)), ptr); \
			APEX_CORE_DEBUG("Added script factory (" STR(script) ")"); \
			return ptr; \
		} \
		Apex::EntityScript* InstantiateScript() override { return new script; } \
		Apex::EntityScript* CloneScript(Apex::EntityScript* from) override { return dynamic_cast<script*>(from) ? new script(*dynamic_cast<script*>(from)) : new script; } \
		void DestroyScript(Apex::EntityScript* _script) override { delete _script; } \
		Type* GetType() const override { return ::GetType<script>(); } \
	}; \
	static Apex::Ref<Apex::AScriptFactory> g_##script = script##Factory::Create();

//class SCRIPT_TEST_API LogScriptFactory : public Apex::AScriptFactory
//{
//public:
//	static decltype(auto) Create()
//	{
//		auto ptr = Apex::Ref<LogScriptFactory>(new LogScriptFactory());
//		Apex::Application::Get().GetResourceManager().Insert<AScriptFactory>(
//			RESNAME(STR(MaterialScript)), ptr);
//		return ptr;
//	}
//
//	Apex::EntityScript* InstantiateScript() override { return new MaterialScript; }
//	void DestroyScript(Apex::EntityScript* _script) override { delete _script; }
//
//private:
//	LogScriptFactory() = default;
//};
//
//static Apex::Ref<Apex::AScriptFactory> g_LogScript = LogScriptFactory::Create();