#pragma once
#include <memory>
#include <vector>
#include <array>
#include <type_traits>
#include <format>
#include <string>
#include <iostream>
#include "EntityManager.hpp"

class Sets
{
public:
	static const uint16_t MAX_ENTITIES = 65535;
};
#define TESTING_ENABLED
#ifdef TESTING_ENABLED
void Assertion(bool condition, const std::string& message) {
	if (!condition) {
		std::cerr << "Assertion failed: " << message << std::endl;
		std::exit(EXIT_FAILURE); // Terminate the program
	}
}
#endif

class ComponentBase {};

template <typename T>
concept ComponentChild = std::is_base_of_v<ComponentBase, T>&& std::derived_from<T, ComponentBase>;

template <ComponentChild T>
class ComponentSet : public Sets
{
private:
	std::array<uint16_t, Sets::MAX_ENTITIES> sparseArray;
	std::vector<std::shared_ptr<T>> components;
public:
	std::vector<uint16_t> entities;
	uint64_t key = 0;
	ComponentSet(uint64_t key);
	size_t count = 0;
	~ComponentSet();
	bool HasEntity(uint16_t entityID);
	void RemoveEntity(uint16_t entityID);
	void AddEntity(uint16_t entityID, const T& data);
	std::shared_ptr<T> GetComponent(uint16_t entityID);
	std::string GetTypeName();
public:
};

template <ComponentChild T>
ComponentSet<T>::ComponentSet(uint64_t key)
{
	this->key = key;
	sparseArray.fill(Sets::MAX_ENTITIES);

}
template <ComponentChild T>
ComponentSet<T>::~ComponentSet() = default;

template <ComponentChild T>
bool ComponentSet<T>::HasEntity(uint16_t entityID)
{
	bool cond1 = entityID < this->sparseArray.size();
	cond1 = cond1 && this->sparseArray[entityID] != Sets::MAX_ENTITIES;
	cond1 = cond1 && this->entities.size() > 0 && this->entities[this->sparseArray[entityID]] == entityID;
	return cond1;

}
template <ComponentChild T>
void ComponentSet<T>::RemoveEntity(uint16_t entityID)
{
	uint16_t pindex = this->sparseArray[entityID];
	Assertion(HasEntity(entityID), std::format("ENTITY: {} DOES NOT HAVE COMPONENT: {}", entityID, typeid(T).name()));

	uint16_t swapID = this->entities[this->entities.size() - 1];
	this->entities.erase(this->entities.begin() + pindex);
	this->components.erase(this->components.begin() + pindex);

	this->sparseArray[swapID] = pindex;
	this->sparseArray[entityID] = Sets::MAX_ENTITIES;
	this->count = this->entities.size();
}
template <ComponentChild T>
void ComponentSet<T>::AddEntity(uint16_t entityID, const T& data)
{
	Assertion(!HasEntity(entityID), std::format("ENTITY: {} ALREADY HAS COMPONENT: {}", entityID, typeid(T).name()));
	this->sparseArray[entityID] = this->entities.size();
	this->entities.push_back(entityID);
	this->components.push_back(std::make_shared<T>(data));
	this->count = this->entities.size();
}
template <ComponentChild T>
std::string ComponentSet<T>::GetTypeName() {
	return typeid(T).name();
}
template <ComponentChild T>
std::shared_ptr<T> ComponentSet<T>::GetComponent(uint16_t entityID)
{
	if (this->HasEntity(entityID)) {
		return this->components[this->sparseArray[entityID]];
	}
	return nullptr;
}