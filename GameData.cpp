#include "GameData.h"

#include <utility>

GameData::GameData()
{
	initializeNewGame();
}

BattleSystem& GameData::getBattleSystem()
{
	return battleSystem;
}

const BattleSystem& GameData::getBattleSystem() const
{
	return battleSystem;
}

void GameData::initializeNewGame()
{
	world.connectLocations(
		"Starter Town",
		"Route 1"
	);

	inventory.addItem(
		"potion",
		"Potion",
		"Restores 20 health.",
		3
	);

	inventory.addItem(
		"capture_orb",
		"Capture Orb",
		"Used to capture wild monsters.",
		5
	);

	auto dialogue =
		std::make_unique<DialogueNode>(
			"Professor: Welcome to MysteryMons!"
		);

	DialogueNode* readyNode =
		dialogue->addChoice(
			"I am ready.",
			"Professor: Your journey begins now."
		);

	dialogue->addChoice(
		"Not yet.",
		"Professor: Return when you are ready."
	);

	readyNode->addChoice(
		"Continue",
		"Professor: Take care of your partner."
	);

	currentDialogue = std::move(dialogue);
}

WorldGraph& GameData::getWorld()
{
	return world;
}

Inventory& GameData::getInventory()
{
	return inventory;
}

Party& GameData::getParty()
{
	return party;
}

MenuStack& GameData::getMenus()
{
	return menus;
}

BattleTurnQueue& GameData::getBattleTurns()
{
	return battleTurns;
}

StatusEffectList&
GameData::getPlayerStatusEffects()
{
	return playerStatusEffects;
}

const WorldGraph& GameData::getWorld() const
{
	return world;
}

const Inventory& GameData::getInventory() const
{
	return inventory;
}

const Party& GameData::getParty() const
{
	return party;
}

const MenuStack& GameData::getMenus() const
{
	return menus;
}

const BattleTurnQueue&
GameData::getBattleTurns() const
{
	return battleTurns;
}

const StatusEffectList&
GameData::getPlayerStatusEffects() const
{
	return playerStatusEffects;
}

DialogueNode* GameData::getCurrentDialogue()
{
	return currentDialogue.get();
}

const DialogueNode*
GameData::getCurrentDialogue() const
{
	return currentDialogue.get();
}

void GameData::setCurrentDialogue(
	std::unique_ptr<DialogueNode> dialogue)
{
	currentDialogue = std::move(dialogue);
}

void GameData::clearCurrentDialogue()
{
	currentDialogue.reset();
}