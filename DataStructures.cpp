#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <unordered_map>
#include <list>
#include "DataStructures.h"

void DataStructures::structures()
{

	// Vector (Party)

	std::vector<std::string> party =
	{
		"Blastoise",
		"Pikachu",
		"Charizard"
	};


	// Queue (Battle Turns)

	std::queue<std::string> turnQueue;

	for (const auto& member : party)
	{
		turnQueue.push(member);
	}


	// Stack (Menu History)

	std::stack<std::string> menuStack;

	menuStack.push("Main Menu");
	menuStack.push("Inventory");
	menuStack.push("Status");


	// Hash Map (Inventory)

	std::unordered_map<std::string, int> inventory;

	inventory["Potion"] = 5;
	inventory["Capture Orb"] = 2;


	// Linked List (Status Effects)

	std::list<std::string> statusEffects;

	statusEffects.push_back("Poison");
	statusEffects.push_back("Swords Dance");

	std::cout << "Data Structures\n";

	std::cout << "Party Members (Vector):\n";
	for (const auto& member : party)
		std::cout << "- " << member << '\n';

	std::cout << "\nCurrent Turn (Queue): ";
	std::cout << turnQueue.front() << '\n';

	std::cout << "\nCurrent Menu (Stack): ";
	std::cout << menuStack.top() << '\n';

	std::cout << "\nPotion Count (Hash Map): ";
	std::cout << inventory["Potion"] << '\n';

	std::cout << "\nStatus Effects (Linked List):\n";
	for (const auto& effect : statusEffects)
		std::cout << "- " << effect << '\n';
}