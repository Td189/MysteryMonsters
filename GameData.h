#pragma once

#include "BattleTurnQueue.h"
#include "DialogueTree.h"
#include "Inventory.h"
#include "MenuStack.h"
#include "Party.h"
#include "StatusEffectList.h"
#include "WorldGraph.h"
#include "BattleSystem.h"
#include <memory>

class GameData
{
public:
	GameData();

	void initializeNewGame();
	BattleSystem& getBattleSystem();
	const BattleSystem& getBattleSystem() const;
	WorldGraph& getWorld();
	Inventory& getInventory();
	Party& getParty();
	MenuStack& getMenus();
	BattleTurnQueue& getBattleTurns();
	StatusEffectList& getPlayerStatusEffects();

	const WorldGraph& getWorld() const;
	const Inventory& getInventory() const;
	const Party& getParty() const;
	const MenuStack& getMenus() const;
	const BattleTurnQueue& getBattleTurns() const;
	const StatusEffectList&
		getPlayerStatusEffects() const;

	DialogueNode* getCurrentDialogue();
	const DialogueNode* getCurrentDialogue() const;

	void setCurrentDialogue(
		std::unique_ptr<DialogueNode> dialogue
	);

	void clearCurrentDialogue();

private:
	WorldGraph world;
	Inventory inventory;
	Party party;
	MenuStack menus;
	BattleTurnQueue battleTurns;
	StatusEffectList playerStatusEffects;
	BattleSystem battleSystem;
	std::unique_ptr<DialogueNode> currentDialogue;
};