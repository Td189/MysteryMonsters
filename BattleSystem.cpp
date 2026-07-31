#include "BattleSystem.h"

#include <algorithm>
#include <iostream>
#include <optional>
#include <random>

namespace
{
	int randomInteger(
		int minimum,
		int maximum)
	{
		static std::mt19937 generator{
			std::random_device{}()
		};

		std::uniform_int_distribution<int> distribution(
			minimum,
			maximum
		);

		return distribution(generator);
	}
}

void BattleSystem::startBattle(
	Monster* playerMonster,
	const Monster& enemyMonster)
{
	if (playerMonster == nullptr)
	{
		std::cerr
			<< "Cannot begin battle without a player monster.\n";

		return;
	}

	if (playerMonster->isFainted())
	{
		std::cerr
			<< playerMonster->name
			<< " has fainted and cannot battle.\n";

		return;
	}

	player = playerMonster;
	enemy = enemyMonster;

	turnQueue.clear();

	state = BattleState::WaitingForPlayer;

	std::cout
		<< "\nA wild "
		<< enemy.name
		<< " appeared!\n";

	std::cout
		<< player->name
		<< " entered the battle!\n";

	std::cout
		<< player->name
		<< " HP: "
		<< player->currentHealth
		<< "/"
		<< player->maximumHealth
		<< '\n';

	std::cout
		<< enemy.name
		<< " HP: "
		<< enemy.currentHealth
		<< "/"
		<< enemy.maximumHealth
		<< "\n\n";
}

// -----------------------------------------------------

void BattleSystem::chooseAttack(
	const std::string& moveName)
{
	if (
		state !=
		BattleState::WaitingForPlayer)
	{
		return;
	}

	if (player == nullptr)
	{
		return;
	}

	if (moveName == "Tackle")
	{
		queueRound(
			"Tackle",
			5
		);
	}
	else if (moveName == "Flame Paw")
	{
		queueRound(
			"Flame Paw",
			8
		);
	}
	else
	{
		std::cout
			<< "Unknown move: "
			<< moveName
			<< '\n';

		return;
	}

	state = BattleState::ProcessingTurns;

	processTurns();
}

// -----------------------------------------------------

void BattleSystem::queueRound(
	const std::string& playerMoveName,
	int playerMovePower)
{
	if (player == nullptr)
	{
		return;
	}

	const int playerDamage =
		calculateDamage(
			*player,
			enemy,
			playerMovePower
		);

	const int enemyMovePower = 5;

	const int enemyDamage =
		calculateDamage(
			enemy,
			*player,
			enemyMovePower
		);

	const BattleTurn playerTurn{
		BattleSide::Player,
		playerMoveName,
		playerDamage
	};

	const BattleTurn enemyTurn{
		BattleSide::Enemy,
		"Scratch",
		enemyDamage
	};

	if (player->speed >= enemy.speed)
	{
		turnQueue.addTurn(playerTurn);
		turnQueue.addTurn(enemyTurn);
	}
	else
	{
		turnQueue.addTurn(enemyTurn);
		turnQueue.addTurn(playerTurn);
	}
}

// -----------------------------------------------------

void BattleSystem::processTurns()
{
	while (!turnQueue.empty())
	{
		const std::optional<BattleTurn> turn =
			turnQueue.takeNextTurn();

		if (!turn.has_value())
		{
			break;
		}

		processTurn(*turn);
		checkBattleResult();

		if (
			state == BattleState::PlayerWon ||
			state == BattleState::PlayerLost)
		{
			turnQueue.clear();
			return;
		}
	}

	state = BattleState::WaitingForPlayer;

	std::cout
		<< "\nChoose another action.\n";
}

// -----------------------------------------------------

void BattleSystem::processTurn(
	const BattleTurn& turn)
{
	if (player == nullptr)
	{
		return;
	}

	if (turn.side == BattleSide::Player)
	{
		if (player->isFainted())
		{
			return;
		}

		std::cout
			<< player->name
			<< " used "
			<< turn.moveName
			<< "!\n";

		enemy.takeDamage(turn.damage);

		std::cout
			<< enemy.name
			<< " took "
			<< turn.damage
			<< " damage.\n";

		std::cout
			<< enemy.name
			<< " HP: "
			<< enemy.currentHealth
			<< "/"
			<< enemy.maximumHealth
			<< "\n\n";
	}
	else
	{
		if (enemy.isFainted())
		{
			return;
		}

		std::cout
			<< enemy.name
			<< " used "
			<< turn.moveName
			<< "!\n";

		player->takeDamage(turn.damage);

		std::cout
			<< player->name
			<< " took "
			<< turn.damage
			<< " damage.\n";

		std::cout
			<< player->name
			<< " HP: "
			<< player->currentHealth
			<< "/"
			<< player->maximumHealth
			<< "\n\n";
	}
}

// -----------------------------------------------------

int BattleSystem::calculateDamage(
	const Monster& attacker,
	const Monster& defender,
	int movePower) const
{
	const int baseDamage =
		attacker.attack +
		movePower -
		defender.defense;

	const int randomBonus =
		randomInteger(0, 2);

	return std::max(
		1,
		baseDamage + randomBonus
	);
}

// -----------------------------------------------------

int BattleSystem::calculateCaptureChance() const
{
	if (enemy.maximumHealth <= 0)
	{
		return 0;
	}

	const float healthRatio =
		static_cast<float>(
			enemy.currentHealth
			) /
		static_cast<float>(
			enemy.maximumHealth
			);

	const int chance =
		static_cast<int>(
			85.0f -
			healthRatio * 70.0f
			);

	return std::clamp(
		chance,
		5,
		95
	);
}

// -----------------------------------------------------

void BattleSystem::enemyFreeAttack()
{
	if (
		!isActive() ||
		player == nullptr)
	{
		return;
	}

	if (enemy.isFainted())
	{
		return;
	}

	const int enemyMovePower = 5;

	const int enemyDamage =
		calculateDamage(
			enemy,
			*player,
			enemyMovePower
		);

	const BattleTurn enemyTurn{
		BattleSide::Enemy,
		"Scratch",
		enemyDamage
	};

	state = BattleState::ProcessingTurns;

	processTurn(enemyTurn);
	checkBattleResult();

	if (
		state != BattleState::PlayerWon &&
		state != BattleState::PlayerLost)
	{
		state = BattleState::WaitingForPlayer;

		std::cout
			<< "\nChoose another action.\n";
	}
}



CaptureResult BattleSystem::attemptCapture(
	Party& party,
	Inventory& inventory)
{
	if (
		state !=
		BattleState::WaitingForPlayer)
	{
		return CaptureResult::NotInBattle;
	}

	if (party.isFull())
	{
		return CaptureResult::PartyFull;
	}

	if (!inventory.useItem("capture_orb"))
	{
		return CaptureResult::NoCaptureItems;
	}

	const int captureChance =
		calculateCaptureChance();

	const int captureRoll =
		randomInteger(1, 100);

	std::cout
		<< "You threw a Capture Orb!\n";

	std::cout
		<< "Capture chance: "
		<< captureChance
		<< "%\n";

	if (captureRoll <= captureChance)
	{
		Monster caughtMonster = enemy;

		if (caughtMonster.currentHealth <= 0)
		{
			caughtMonster.currentHealth = 1;
		}

		if (!party.addMonster(caughtMonster))
		{
			return CaptureResult::PartyFull;
		}

		state = BattleState::PlayerWon;
		turnQueue.clear();

		std::cout
			<< "You caught "
			<< caughtMonster.name
			<< "!\n";

		return CaptureResult::Caught;
	}

	std::cout
		<< enemy.name
		<< " escaped from the Capture Orb!\n";

	enemyFreeAttack();

	return CaptureResult::Failed;
}

// -----------------------------------------------------

void BattleSystem::beginSwitch(
	const Party& party)
{
	if (
		state !=
		BattleState::WaitingForPlayer)
	{
		return;
	}

	if (player == nullptr)
	{
		return;
	}

	bool hasAvailableMonster = false;

	for (
		std::size_t index = 0;
		index < party.size();
		++index)
	{
		const Monster* monster =
			party.getMonster(index);

		if (
			monster != nullptr &&
			monster != player &&
			!monster->isFainted())
		{
			hasAvailableMonster = true;
			break;
		}
	}

	if (!hasAvailableMonster)
	{
		std::cout
			<< "You do not have another healthy "
			<< "monster to switch to.\n";

		return;
	}

	state = BattleState::ChoosingSwitch;

	std::cout
		<< "\nChoose a monster:\n";

	for (
		std::size_t index = 0;
		index < party.size();
		++index)
	{
		const Monster* monster =
			party.getMonster(index);

		if (monster == nullptr)
		{
			continue;
		}

		std::cout
			<< index + 1
			<< ". "
			<< monster->name
			<< " HP: "
			<< monster->currentHealth
			<< "/"
			<< monster->maximumHealth;

		if (monster == player)
		{
			std::cout << " (Active)";
		}
		else if (monster->isFainted())
		{
			std::cout << " (Fainted)";
		}

		std::cout << '\n';
	}

	std::cout
		<< "Press 1-6 to choose a monster.\n"
		<< "Press Escape to cancel.\n";
}

SwitchResult BattleSystem::switchMonster(
	Party& party,
	std::size_t index)
{
	if (
		state !=
		BattleState::ChoosingSwitch)
	{
		return SwitchResult::NotChoosing;
	}

	Monster* selectedMonster =
		party.getMonster(index);

	if (selectedMonster == nullptr)
	{
		std::cout
			<< "There is no monster in that party slot.\n";

		return SwitchResult::InvalidIndex;
	}

	if (selectedMonster == player)
	{
		std::cout
			<< selectedMonster->name
			<< " is already battling.\n";

		return SwitchResult::SameMonster;
	}

	if (selectedMonster->isFainted())
	{
		std::cout
			<< selectedMonster->name
			<< " has fainted and cannot battle.\n";

		return SwitchResult::MonsterFainted;
	}

	if (player != nullptr)
	{
		std::cout
			<< player->name
			<< ", come back!\n";
	}

	player = selectedMonster;

	std::cout
		<< "Go, "
		<< player->name
		<< "!\n";

	std::cout
		<< player->name
		<< " HP: "
		<< player->currentHealth
		<< "/"
		<< player->maximumHealth
		<< "\n\n";

	/*
		Switching uses the player's turn,
		so the enemy receives a free attack.
	*/
	enemyFreeAttack();

	return SwitchResult::Switched;
}

// -----------------------------------------------------

void BattleSystem::cancelSwitch()
{
	if (
		state !=
		BattleState::ChoosingSwitch)
	{
		return;
	}

	state = BattleState::WaitingForPlayer;

	std::cout
		<< "Switch cancelled.\n"
		<< "Choose another action.\n";
}

void BattleSystem::checkBattleResult()
{
	if (player == nullptr)
	{
		state = BattleState::PlayerLost;
		return;
	}

	if (enemy.isFainted())
	{
		state = BattleState::PlayerWon;

		std::cout
			<< enemy.name
			<< " fainted!\n";

		std::cout
			<< "You won the battle!\n";
	}
	else if (player->isFainted())
	{
		state = BattleState::PlayerLost;

		std::cout
			<< player->name
			<< " fainted!\n";

		std::cout
			<< "You lost the battle.\n";
	}
}

// -----------------------------------------------------

bool BattleSystem::isActive() const
{
	return
		state != BattleState::Inactive &&
		state != BattleState::PlayerWon &&
		state != BattleState::PlayerLost;
}

BattleState BattleSystem::getState() const
{
	return state;
}

const Monster& BattleSystem::getEnemyMonster() const
{
	return enemy;
}
Monster* BattleSystem::getPlayerMonster()
{
	return player;
}

// -----------------------------------------------------

void BattleSystem::endBattle()
{
	turnQueue.clear();

	player = nullptr;

	state = BattleState::Inactive;
}