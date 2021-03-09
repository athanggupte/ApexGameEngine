#pragma once
#include <vector>
#include <functional>

#define COMPONENT(_typename_) struct _typename_ : public Apex::ECS::Component<_typename_>

namespace Apex::ECS {

	struct AbstractComponent; // Forward Declaration

	struct AbstractComponent
	{
		uint32_t entityID;

		static uint32_t RegisterComponentType(const std::string& typeName);

		template <typename T, typename = typename std::enable_if_t<std::is_base_of_v<AbstractComponent, T>>>
		inline static std::string_view GetDebugName() { return std::string_view{ componentTypes[T::TYPE_ID] }; }

		//static uint32_t RegisterComponentType(ComponentCreateFn, ComponentFreeFn, size_t);
		//inline static ComponentCreateFn GetTypeCreateFunction(uint32_t typeID) { return std::get<0>(componentTypes[typeID]); }
		//inline static ComponentFreeFn GetTypeFreeFunction(uint32_t typeID) { return std::get<1>(componentTypes[typeID]); }
		//inline static size_t GetTypeSize(uint32_t typeID) { return std::get<2>(componentTypes[typeID]); }

	//private:
		inline static std::vector<std::string> componentTypes;
		//static std::vector<std::tuple<ComponentCreateFn, ComponentFreeFn, size_t>> componentTypes;
	};

	template <typename T>
	using ComponentCreateFn = std::function<T&(std::vector<T>& memory, uint32_t entity, T component)>;

	template <typename T>
	using ComponentFreeFn = std::function<uint32_t(T* component)>;

	template <typename T>
	struct Component : public AbstractComponent
	{
		static const uint32_t TYPE_ID;
		static const size_t TYPE_SIZE;

		static const ComponentCreateFn<T> CREATE_FN;
		static const ComponentFreeFn<T> FREE_FN;

		inline static uint32_t COUNT{ 0 };
		inline static std::vector<T> COMPONENT_LIST;
	};
	
	/*! @note Static members of templates can be defined in header file only.
	 *        Since they are template initialized the compiler will make sure
	 *        they're only defined once.
	 */

	template <typename T>
	const uint32_t Component<T>::TYPE_ID{ AbstractComponent::RegisterComponentType(typeid(T).name()) };

	template <typename T>
	const size_t Component<T>::TYPE_SIZE{ sizeof(T) };

//	template <typename T>
//	uint32_t Component<T>::COUNT;
//
//	template <typename T>
//	std::vector<T> Component<T>::COMPONENT_LIST;

	template <typename T>
	const ComponentCreateFn<T> Component<T>::CREATE_FN = [](std::vector<T>& memory, uint32_t entity, T component) -> decltype(auto) {
		auto& it = memory.emplace_back(std::move(component));
		COUNT = memory.size();
		APEX_CORE_DEBUG("Created component <{}>", AbstractComponent::GetDebugName<T>());
		return it;
	};

	template <typename T>
	const ComponentFreeFn<T> Component<T>::FREE_FN = [](auto comp) {
		T* component = (T*)comp;
		component->~T();
	};
	
	// Test Component
	COMPONENT(TestComponent)
	{
		
	};
}