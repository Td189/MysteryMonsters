#pragma once

#include "GameTypes.h"

#include <cstddef>
#include <vector>

class Party
{
public:
	static constexpr std::size_t MaximumPartySize = 6;

	bool addMonster(const Monster& monster);
	bool removeMonster(std::size_t index);

	bool swapMonsters(
		std::size_t first,
		std::size_t second
	);

	void healAll();

	bool isFull() const;
	std::size_t size() const;

	Monster* getMonster(std::size_t index);

	const Monster* getMonster(
		std::size_t index
	) const;

	const std::vector<Monster>& getAll() const;

private:
	std::vector<Monster> monsters;
};