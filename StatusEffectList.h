#pragma once

#include "GameTypes.h"

#include <list>
#include <string>

class StatusEffectList
{
public:
	void addEffect(const StatusEffect& effect);

	bool hasEffect(const std::string& name) const;

	int processTurn();

	void clear();

	const std::list<StatusEffect>& getAll() const;

private:
	std::list<StatusEffect> effects;
};