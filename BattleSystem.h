#pragma once

#include "BattleTurnQueue.h"
#include "GameTypes.h"
#include "Inventory.h"
#include "Party.h"

#include <cstddef>
#include <string>

enum class BattleState
{
	Inactive,
	WaitingForPlayer,
	ChoosingSwitch,
	ProcessingTurns,
	PlayerWon,
	PlayerLost
};

enum class SwitchResult
{
	NotChoosing,
	InvalidIndex,
	SameMonster,
	MonsterFainted,
	Switched
};

class BattleSystem
{
public:
	void startBattle(
		Monster* playerMonster,
		const Monster& enemyMonster
	);

	void chooseAttack(
		const std::string& moveName
	);

	CaptureResult attemptCapture(
		Party& party,
		Inventory& inventory
	);

	void beginSwitch(
		const Party& party
	);

	SwitchResult switchMonster(
		Party& party,
		std::size_t index
	);

	void cancelSwitch();

	void processTurns();

	bool isActive() const;

	BattleState getState() const;

	Monster* getPlayerMonster();

	const Monster& getEnemyMonster() const;

	void endBattle();

private:
	int calculateDamage(
		const Monster& attacker,
		const Monster& defender,
		int movePower
	) const;

	int calculateCaptureChance() const;

	void queueRound(
		const std::string& playerMoveName,
		int playerMovePower
	);

	void processTurn(
		const BattleTurn& turn
	);

	void enemyFreeAttack();

	void checkBattleResult();

	Monster* player = nullptr;

	Monster enemy{
		"Unknown",
		1,
		1,
		1,
		1,
		1,
		1
	};

	BattleTurnQueue turnQueue;

	BattleState state =
		BattleState::Inactive;
};