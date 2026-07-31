#pragma once

#include "Monster.h"

#include <cstddef>
#include <string>
#include <vector>

struct MonsterSpecies
{
	std::string id;
	std::string displayName;

	int baseHealth;
	int baseAttack;
	int baseDefense;
	int baseSpeed;

	std::string imageFolder;
	std::string frontPrefix;
	std::string backPrefix;

	std::size_t frontFrameCount;
	std::size_t backFrameCount;
};

class MonsterCatalog
{
public:
	static const MonsterSpecies* findSpecies(
		const std::string& speciesId
	);

	static Monster createMonster(
		const std::string& speciesId,
		int level
	);

	static Monster createRandomWildMonster(
		int minimumLevel,
		int maximumLevel
	);

private:
	static const std::vector<MonsterSpecies>& getSpecies();
};