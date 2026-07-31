#pragma once

#include "GameTypes.h"

#include <string>
#include <unordered_map>

class Inventory
{
public:
	void addItem(
		const std::string& itemId,
		const std::string& name,
		const std::string& description,
		int amount = 1
	);

	bool removeItem(
		const std::string& itemId,
		int amount = 1
	);

	bool useItem(
		const std::string& itemId
	);

	int getQuantity(
		const std::string& itemId
	) const;

	const InventoryItem* getItem(
		const std::string& itemId
	) const;

	const std::unordered_map<
		std::string,
		InventoryItem
	>& getAllItems() const;

private:
	std::unordered_map<
		std::string,
		InventoryItem
	> items;
};