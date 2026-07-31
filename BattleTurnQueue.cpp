#include "BattleTurnQueue.h"

void BattleTurnQueue::addTurn(
	const BattleTurn& turn)
{
	turns.push(turn);
}

std::optional<BattleTurn>
BattleTurnQueue::takeNextTurn()
{
	if (turns.empty())
	{
		return std::nullopt;
	}

	BattleTurn next = turns.front();
	turns.pop();

	return next;
}

bool BattleTurnQueue::empty() const
{
	return turns.empty();
}

void BattleTurnQueue::clear()
{
	while (!turns.empty())
	{
		turns.pop();
	}
}