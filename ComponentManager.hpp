#pragma once
#include <functional>
#include <map>
#include "ComponentSet.hpp"

typedef std::function<void(uint16_t)> ComponentRemoveEntity;

template <typename T>
class Component : public ComponentBase { protected: Component(); };


class ComponentManager
{
public:
	inline static std::vector<ComponentRemoveEntity> allEntityRemoves;
	inline static uint16_t componentCount;
	inline static std::vector<std::shared_ptr<Sets>> allComponentSets;
	inline static std::map<std::string, uint64_t> componentKeys;

	template <ComponentChild T>
	static std::shared_ptr<ComponentSet<T>> CreateComponentSet();

	template <ComponentChild T>
	static std::shared_ptr<ComponentSet<T>> GetComponentSet();


	template <ComponentChild T>
	static uint64_t GetComponentKey();

};


template <ComponentChild T>
std::shared_ptr<ComponentSet<T>> ComponentManager::CreateComponentSet()
{
	if (ComponentManager::GetComponentKey<T>() == 65) {
	auto componentName = typeid(T).name();
	Assertion(ComponentManager::componentCount <= 64, "NOT ENOUGH COMPONENT SPACE");
		auto instanceOfComponentSet = std::shared_ptr<ComponentSet<T>>(new ComponentSet<T>(ComponentManager::componentCount));
		ComponentManager::allComponentSets.push_back(instanceOfComponentSet);
		ComponentManager::componentKeys[componentName] = ComponentManager::componentCount;


		ComponentManager::allEntityRemoves.push_back(
			std::bind(&ComponentSet<T>::RemoveEntity, instanceOfComponentSet, std::placeholders::_1)
		);

		ComponentManager::componentCount += 1;

		return instanceOfComponentSet;
	}
	return ComponentManager::GetComponentSet<T>();
}

template <ComponentChild T>
std::shared_ptr<ComponentSet<T>> ComponentManager::GetComponentSet()
{
	std::string comp = typeid(T).name();
	auto key = ComponentManager::GetComponentKey<T>();

	Assertion(key != 65, std::format("COMPONENT: {} HAS NOT BEEN CREATED", comp));

	auto basePtr = ComponentManager::allComponentSets[key];
	std::shared_ptr<ComponentSet<T>>  result = std::reinterpret_pointer_cast<ComponentSet<T>>(basePtr);
	return result;
}


template <ComponentChild T>
uint64_t ComponentManager::GetComponentKey() {
	std::string comp = typeid(T).name();
	bool isNotFound = ComponentManager::componentKeys.find(comp) == ComponentManager::componentKeys.end();
	if (isNotFound) return 65;
	return ComponentManager::componentKeys[typeid(T).name()];
}

template <typename T>
Component<T>::Component() {
	ComponentManager::CreateComponentSet<T>();
}
