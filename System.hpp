#pragma once
#include <vector>
#include "EntityManager.hpp"
#include <type_traits>
#include <memory>
class SystemBase {
public:
	bool isActive = true;
	virtual void Run(float deltaTime) = 0;
	virtual ~SystemBase() = default;
};

template <ComponentChild T>
class System : SystemBase {
protected:
	std::shared_ptr<ComponentSet<T>> data();
	virtual void Execute(std::vector<uint16_t> entities, float deltatime) = 0;
public:
	void Run(float deltaTime);
};

template <ComponentChild T>
std::shared_ptr<ComponentSet<T>> System<T>::data() {
	return ComponentManager::GetComponentSet<T>();
};

template <ComponentChild T>
void System<T>::Run(float deltaTime) {
	if (this->isActive) {
		this->Execute(this->data()->entities, deltaTime);
	}
}
