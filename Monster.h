#pragma once

#include <string>

struct Monster
{
	std::string speciesId;
	std::string name;
	std::string imageFolder;
	std::size_t animationFrameCount = 1;
	int level = 1;
	int currentHealth = 1;
	int maximumHealth = 1;
	int attack = 1;
	int defense = 1;
	int speed = 1;

	Monster() = default;

	Monster(
		const std::string& speciesId,
		const std::string& name,
		int level,
		int currentHealth,
		int maximumHealth,
		int attack,
		int defense,
		int speed)
		: speciesId(speciesId),
		name(name),
		level(level),
		currentHealth(currentHealth),
		maximumHealth(maximumHealth),
		attack(attack),
		defense(defense),
		speed(speed)
	{
	}

	bool isFainted() const
	{
		return currentHealth <= 0;
	}
};