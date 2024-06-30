#pragma once

#include "ComponentManager.hpp"
#include "EntityManager.hpp"
#include <array>

struct BitTracker
{
public:
	static const uint64_t TOTAL_BITS = 64;
	uint64_t bits = 0;
	bool Has(uint64_t index) { return (bits & (1ULL << index)) == (1ULL << index); };
	void Add(uint64_t index) {
		if (!Has(index)) {
			bits = bits | (1ULL << index);
		}
	}
	void Remove(uint64_t index) {
		if (Has(index)) {
			bits = bits ^ (1ULL << index);
		}
	}
	BitTracker(uint64_t bits) {
		this->bits = bits;
	}
	BitTracker() { this->bits = 0; }
};


struct Entity
{
private:
	friend class EntityManager;
	Entity(uint16_t id);

public:
	uint16_t _id;
	template <ComponentChild T>
	bool HasComponent();
	template <ComponentChild T>
	std::shared_ptr<Entity> AddComponent(const T& data);
	template <ComponentChild T>
	void RemoveComponent();
	template <ComponentChild T>
	std::shared_ptr<T> GetComponent();
	void Destroy();
	bool Equals(std::shared_ptr<Entity> other);
};

class EntityManager
{
public:
	inline static std::array<std::shared_ptr<BitTracker>, Sets::MAX_ENTITIES> activeEntityBits;
	inline static std::array<std::shared_ptr<Entity>, Sets::MAX_ENTITIES> activeEntities;
	inline static std::vector<uint16_t> deadEntities;
	inline static uint16_t nextEntityId = 0;
	inline static uint16_t entityCount = 0;
	static std::shared_ptr<Entity> CreateEntity();
	template<ComponentChild T>
	static void AddKey(uint16_t id);
	template<ComponentChild T>
	static void RemoveKey(uint16_t id);
	static void DestroyEntity(uint16_t id);
	static std::shared_ptr<Entity> GetEntity(uint16_t id);
};

#pragma region Entity

Entity::Entity(uint16_t id) {
	this->_id = id;
}

bool Entity::Equals(std::shared_ptr<Entity> other) {
	return this->_id == other->_id;
}

void Entity::Destroy() {
	EntityManager::DestroyEntity(this->_id);
}

template <ComponentChild T>
bool Entity::HasComponent() {
	return EntityManager::activeEntityBits[this->_id]->Has(ComponentManager::GetComponentKey<T>());
}

template <ComponentChild T>
std::shared_ptr<Entity> Entity::AddComponent(const T& data) {
	auto componentSet = ComponentManager::GetComponentSet<T>();
	Assertion(!componentSet->HasEntity(this->_id), std::format("ENTITY: {} ALREADY HAS COMPONENT: {}", this->_id, typeid(T).name()));
	componentSet->AddEntity(this->_id, data);
	EntityManager::AddKey<T>(this->_id);
	return EntityManager::GetEntity(this->_id);
}

template <ComponentChild T>
void Entity::RemoveComponent() {
	auto componentSet = ComponentManager::GetComponentSet<T>();
	Assertion(componentSet->HasEntity(this->_id), std::format("ENTITY: {} DOES NOT HAVE COMPONENT: {}", this->_id, typeid(T).name()));
	componentSet->RemoveEntity(this->_id);
	EntityManager::RemoveKey<T>(this->_id);
}

template <ComponentChild T>
std::shared_ptr<T> Entity::GetComponent() {
	auto componentSet = ComponentManager::GetComponentSet<T>();
	return componentSet->GetComponent(this->_id);
}

#pragma endregion

#pragma region EntityManager

std::shared_ptr<Entity> EntityManager::CreateEntity()
{
	uint16_t id = EntityManager::nextEntityId;
	if (EntityManager::deadEntities.size() > 0) {
		id = deadEntities.back();
		deadEntities.pop_back();
	}
	else {
		EntityManager::nextEntityId++;
	}

	EntityManager::activeEntityBits[id] = std::make_shared<BitTracker>(BitTracker());
	EntityManager::activeEntities[id] = std::shared_ptr<Entity>(new Entity(id));
	EntityManager::entityCount++;
	return EntityManager::activeEntities[id];
}

std::shared_ptr<Entity> EntityManager::GetEntity(uint16_t id)
{
	return EntityManager::activeEntities[id];
}
template<ComponentChild T>
void EntityManager::AddKey(uint16_t id) {
	auto entityBits = EntityManager::activeEntityBits[id];
	entityBits->Add(ComponentManager::GetComponentKey<T>());
};

template<ComponentChild T>
void EntityManager::RemoveKey(uint16_t id) {
	auto entityBits = EntityManager::activeEntityBits[id];
	auto key = ComponentManager::GetComponentKey<T>();
	entityBits->Remove(ComponentManager::GetComponentKey<T>());
};

void EntityManager::DestroyEntity(uint16_t id)
{
	EntityManager::entityCount--;
	EntityManager::deadEntities.push_back(id);
	auto bits = EntityManager::activeEntityBits[id];
	for (uint64_t i = 0; i < BitTracker::TOTAL_BITS; i++)
	{
		if (bits->Has(i))
		{
			bits->Remove(i);
			auto ptr = ComponentManager::allEntityRemoves[i];
			ptr(id);
		}
	}
	EntityManager::activeEntityBits[id] = nullptr;
}
#pragma endregion