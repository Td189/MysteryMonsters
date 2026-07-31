#pragma once

#include "GameTypes.h"

#include <optional>
#include <queue>

class BattleTurnQueue
{
public:
	void addTurn(const BattleTurn& turn);

	std::optional<BattleTurn> takeNextTurn();

	bool empty() const;

	void clear();

private:
	std::queue<BattleTurn> turns;
};