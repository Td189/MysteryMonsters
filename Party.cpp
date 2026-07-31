#include "Party.h"

#include <utility>

bool Party::addMonster(const Monster& monster)
{
	if (isFull())
	{
		return false;
	}

	monsters.push_back(monster);
	return true;
}

bool Party::removeMonster(std::size_t index)
{
	if (index >= monsters.size())
	{
		return false;
	}

	monsters.erase(
		monsters.begin() +
		static_cast<std::ptrdiff_t>(index)
	);

	return true;
}

bool Party::swapMonsters(
	std::size_t first,
	std::size_t second)
{
	if (
		first >= monsters.size() ||
		second >= monsters.size()
		)
	{
		return false;
	}

	std::swap(
		monsters[first],
		monsters[second]
	);

	return true;
}

void Party::healAll()
{
	for (Monster& monster : monsters)
	{
		monster.currentHealth =
			monster.maximumHealth;
	}
}

bool Party::isFull() const
{
	return monsters.size() >= MaximumPartySize;
}

std::size_t Party::size() const
{
	return monsters.size();
}

Monster* Party::getMonster(
	std::size_t index)
{
	if (index >= monsters.size())
	{
		return nullptr;
	}

	return &monsters[index];
}

const Monster* Party::getMonster(
	std::size_t index) const
{
	if (index >= monsters.size())
	{
		return nullptr;
	}

	return &monsters[index];
}

const std::vector<Monster>& Party::getAll() const
{
	return monsters;
}