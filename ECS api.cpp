// ECS api.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "EntityManager.hpp"
#include "System.hpp"

class Vec2 : public Component<Vec2> {
public:
	Vec2() {};
	int x = 0;
	int y = 0;
};

class Player : public Component<Player> {

};

class MovementSystem : public System<Vec2> {
public:
	MovementSystem() {};
protected:
	void Execute(std::vector<uint16_t> entities, float deltatime) override {
		for (int i = 0; i < entities.size(); i++)
		{
			auto entity = EntityManager::GetEntity(entities[i]);
			auto component = entity->GetComponent<Vec2>();
			component->x++;
			component->y--;
			std::cout << component->x << " " << component->y << "\n";
		}
	}
};

int main()
{
	EntityManager::CreateEntity()->AddComponent<Vec2>(Vec2());
	MovementSystem move;
	while (true) {
		move.Run(0.0f);
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
