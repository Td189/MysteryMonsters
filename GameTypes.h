#pragma once

#include <string>


// Monster data


struct Monster
{
	std::string name;

	int level = 1;

	int currentHealth = 20;
	int maximumHealth = 20;

	int attack = 5;
	int defense = 3;
	int speed = 5;

	bool isFainted() const
	{
		return currentHealth <= 0;
	}

	void takeDamage(int damage)
	{
		currentHealth -= damage;

		if (currentHealth < 0)
		{
			currentHealth = 0;
		}
	}

	void heal(int amount)
	{
		currentHealth += amount;

		if (currentHealth > maximumHealth)
		{
			currentHealth = maximumHealth;
		}
	}
};
enum class CaptureResult
{
	NotInBattle,
	NoCaptureItems,
	PartyFull,
	Failed,
	Caught
};

// Battle data


enum class BattleSide
{
	Player,
	Enemy
};

struct BattleTurn
{
	BattleSide side = BattleSide::Player;

	std::string moveName;

	int damage = 0;
};


// Inventory data


struct InventoryItem
{
	std::string name;

	std::string description;

	int quantity = 0;
};


// Status-effect data


struct StatusEffect
{
	std::string name;

	int turnsRemaining = 0;

	int damagePerTurn = 0;
};


// Menu types


enum class MenuType
{
	Pause,
	Main,
	Inventory,
	Party,
	Settings,
	Battle
};