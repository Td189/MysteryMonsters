#include "MonsterCatalog.h"

#include <algorithm>
#include <iostream>
#include <random>

namespace
{
	std::mt19937& getRandomEngine()
	{
		static std::random_device randomDevice;
		static std::mt19937 engine(randomDevice());

		return engine;
	}
}

const std::vector<MonsterSpecies>& MonsterCatalog::getSpecies()
{
	static const std::vector<MonsterSpecies> species =
	{
		{
			"mossling",
			"Mossling",

			18, // Health
			6,  // Attack
			3,  // Defense
			4,  // Speed

			"photos/Mossling",
			"front",
			"back",

			4, // Front frames
			4  // Back frames
		},

		{
			"emberfox",
			"EmberFox",

			20,
			8,
			4,
			7,

			"photos/EmberFox",
			"front",
			"back",

			4,
			4
		},

		{
			"aquafin",
			"Aquafin",

			22,
			7,
			6,
			5,

			"photos/Aquafin",
			"front",
			"back",

			4,
			4
		}
	};

	return species;
}

const MonsterSpecies* MonsterCatalog::findSpecies(
	const std::string& speciesId)
{
	const std::vector<MonsterSpecies>& species =
		getSpecies();

	const auto result = std::find_if(
		species.begin(),
		species.end(),
		[&speciesId](const MonsterSpecies& entry)
		{
			return entry.id == speciesId;
		}
	);

	if (result == species.end())
	{
		return nullptr;
	}

	return &(*result);
}

Monster MonsterCatalog::createMonster(
	const std::string& speciesId,
	int level)
{
	const MonsterSpecies* species =
		findSpecies(speciesId);

	if (species == nullptr)
	{
		std::cerr
			<< "Unknown monster species: "
			<< speciesId
			<< '\n';

		return {};
	}

	level = std::max(level, 1);

	// Simple level scaling. You can change these formulas later.
	const int health =
		species->baseHealth + ((level - 1) * 2);

	const int attack =
		species->baseAttack + ((level - 1) / 2);

	const int defense =
		species->baseDefense + ((level - 1) / 2);

	const int speed =
		species->baseSpeed + ((level - 1) / 3);

	return Monster{
		species->id,
		species->displayName,
		level,
		health,
		health,
		attack,
		defense,
		speed
	};
}

Monster MonsterCatalog::createRandomWildMonster(
	int minimumLevel,
	int maximumLevel)
{
	const std::vector<MonsterSpecies>& species =
		getSpecies();

	if (species.empty())
	{
		std::cerr
			<< "Monster catalog contains no species.\n";

		return {};
	}

	if (minimumLevel > maximumLevel)
	{
		std::swap(minimumLevel, maximumLevel);
	}

	minimumLevel = std::max(minimumLevel, 1);
	maximumLevel = std::max(maximumLevel, minimumLevel);

	std::uniform_int_distribution<std::size_t>
		speciesDistribution(
			0,
			species.size() - 1
		);

	std::uniform_int_distribution<int>
		levelDistribution(
			minimumLevel,
			maximumLevel
		);

	const MonsterSpecies& randomSpecies =
		species[
			speciesDistribution(getRandomEngine())
		];

	const int randomLevel =
		levelDistribution(getRandomEngine());

	return createMonster(
		randomSpecies.id,
		randomLevel
	);
}