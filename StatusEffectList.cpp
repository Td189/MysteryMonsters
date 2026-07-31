#include "StatusEffectList.h"

void StatusEffectList::addEffect(
	const StatusEffect& effect)
{
	effects.push_back(effect);
}

bool StatusEffectList::hasEffect(
	const std::string& name) const
{
	for (const StatusEffect& effect : effects)
	{
		if (effect.name == name)
		{
			return true;
		}
	}

	return false;
}

int StatusEffectList::processTurn()
{
	int totalDamage = 0;

	for (
		auto iterator = effects.begin();
		iterator != effects.end();)
	{
		totalDamage += iterator->damagePerTurn;
		--iterator->turnsRemaining;

		if (iterator->turnsRemaining <= 0)
		{
			iterator = effects.erase(iterator);
		}
		else
		{
			++iterator;
		}
	}

	return totalDamage;
}

void StatusEffectList::clear()
{
	effects.clear();
}

const std::list<StatusEffect>&
StatusEffectList::getAll() const
{
	return effects;
}